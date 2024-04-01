#include "Editor.hpp"

#include <Aka/Layer/ImGuiLayer.h>

using namespace aka;

static float vertices[] = {
	-1.0f,  1.0f, -1.0f,	0.f, 1.f,
	-1.0f, -1.0f, -1.0f,	0.f, 0.f,
	 1.0f, -1.0f, -1.0f,	1.f, 0.f,
	 1.0f, -1.0f, -1.0f,	1.f, 0.f,
	 1.0f,  1.0f, -1.0f,	1.f, 1.f,
	-1.0f,  1.0f, -1.0f,	0.f, 1.f,

	-1.0f, -1.0f,  1.0f,	0.f, 1.f,
	-1.0f, -1.0f, -1.0f,	0.f, 0.f,
	-1.0f,  1.0f, -1.0f,	1.f, 0.f,
	-1.0f,  1.0f, -1.0f,	1.f, 0.f,
	-1.0f,  1.0f,  1.0f,	1.f, 1.f,
	-1.0f, -1.0f,  1.0f,	0.f, 1.f,

	 1.0f, -1.0f, -1.0f,	0.f, 0.f,
	 1.0f, -1.0f,  1.0f,	0.f, 1.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	 1.0f,  1.0f, -1.0f,	1.f, 0.f,
	 1.0f, -1.0f, -1.0f,	0.f, 0.f,

	-1.0f, -1.0f,  1.0f,	0.f, 0.f,
	-1.0f,  1.0f,  1.0f,	0.f, 1.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	 1.0f, -1.0f,  1.0f,	1.f, 0.f,
	-1.0f, -1.0f,  1.0f,	0.f, 0.f,

	-1.0f,  1.0f, -1.0f,	0.f, 0.f,
	 1.0f,  1.0f, -1.0f,	1.f, 0.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	 1.0f,  1.0f,  1.0f,	1.f, 1.f,
	-1.0f,  1.0f,  1.0f,	0.f, 1.f,
	-1.0f,  1.0f, -1.0f,	0.f, 0.f,

	-1.0f, -1.0f, -1.0f,	0.f, 0.f,
	-1.0f, -1.0f,  1.0f,	0.f, 1.f,
	 1.0f, -1.0f, -1.0f,	1.f, 0.f,
	 1.0f, -1.0f, -1.0f,	1.f, 0.f,
	-1.0f, -1.0f,  1.0f,	0.f, 1.f,
	 1.0f, -1.0f,  1.0f,	1.f, 1.f,
};

struct UniformBuffer {
	mat4f model;
	mat4f view;
	mat4f proj;
};

Editor::Editor(const aka::Config& cfg) :
	aka::Application(cfg)
{
	// Add ImGuiLayer to use imgui. Will take care of everything render related.
	// Need to be called in Application::onRender(...) function.
	aka::ImGuiLayer* imgui = getRoot().addLayer<aka::ImGuiLayer>();
}

Editor::~Editor()
{
}

void Editor::onCreate(int argc, char* argv[])
{
	// Register our shader in registry for hot reload & co.
	ShaderKey vertexShader = ShaderKey::generate(AssetPath("../shaders/shader.vert"), gfx::ShaderType::Vertex);
	ShaderKey fragShader = ShaderKey::generate(AssetPath("../shaders/shader.frag"), gfx::ShaderType::Fragment);
	ProgramKey programKey = ProgramKey{}.add(vertexShader).add(fragShader);
	program()->add(programKey, graphic());
	// Create UBO
	UniformBuffer uboData;
	uboData.model = mat4f::identity();
	uboData.view = mat4f::lookAtView(point3f(3), point3f(0));
	uboData.proj = mat4f::perspective(anglef::degree(60.f), width() / (float)height(), 0.1f, 100.f);
	m_ubo = graphic()->createBuffer("UBO", gfx::BufferType::Uniform, sizeof(UniformBuffer), gfx::BufferUsage::Dynamic, gfx::BufferCPUAccess::Write, &uboData);
	// Create mesh
	gfx::VertexBufferLayout vertexLayout;
	vertexLayout.add(gfx::VertexSemantic::Position, gfx::VertexFormat::Float, gfx::VertexType::Vec3);
	vertexLayout.add(gfx::VertexSemantic::TexCoord0, gfx::VertexFormat::Float, gfx::VertexType::Vec2);
	m_vertexBuffer = graphic()->createBuffer("Vertices", gfx::BufferType::Vertex, sizeof(vertices), gfx::BufferUsage::Default, gfx::BufferCPUAccess::None, vertices);
	
	gfx::ShaderBindingState state = gfx::ShaderBindingState().add(gfx::ShaderBindingType::UniformBuffer, gfx::ShaderMask::Vertex);
	gfx::ShaderPipelineLayout layout;
	layout.addSet(state);
	m_program = program()->get(programKey);
	// Descriptors
	m_pool = graphic()->createDescriptorPool("DescriptorPool", state, 1);
	m_set = graphic()->allocateDescriptorSet("DescriptorSet", state, m_pool);
	gfx::DescriptorUpdate update = gfx::DescriptorUpdate::uniformBuffer(0, 0, m_ubo);
	graphic()->update(m_set, &update, 1);
	// Renderpass
	m_renderPass = graphic()->createBackbufferRenderPass();
	m_backbuffer = graphic()->createBackbuffer(m_renderPass);
	// Pipeline
	m_pipeline = graphic()->createGraphicPipeline(
		"GraphicPipeline",
		m_program,
		gfx::PrimitiveType::Triangles,
		layout,
		graphic()->get(m_renderPass)->state,
		gfx::VertexState{}.add(vertexLayout),
		gfx::ViewportStateBackbuffer,
		gfx::DepthStateLessEqual,
		gfx::StencilStateDefault,
		gfx::CullStateCCW,
		gfx::BlendStateDefault,
		gfx::FillStateFill
	);
}

void Editor::onDestroy()
{
	graphic()->destroy(m_ubo);
	graphic()->destroy(m_vertexBuffer);
	graphic()->destroy(m_vertex);
	graphic()->destroy(m_fragment);
	graphic()->destroy(m_program);
	graphic()->destroy(m_pipeline);
	graphic()->destroy(m_pool);
	graphic()->destroy(m_renderPass);
}

void Editor::onFixedUpdate(aka::Time deltaTime)
{
}

void Editor::onUpdate(aka::Time deltaTime)
{
	// Get user input & close on escape
	if (platform()->keyboard().down(aka::KeyboardKey::Escape))
		aka::EventDispatcher<aka::QuitEvent>::emit();
	// Rotate the cube
	m_rotation += anglef::radian(deltaTime.seconds());
}


void Editor::onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle _frame)
{
	gfx::GraphicDevice* device = graphic();
	vec2f size = aka::vec2f(300.f);
	vec2f position = aka::vec2f(
		width() / 2.f - size.x / 2.f,
		height() / 2.f - size.y / 2.f
	);
	mat3f transform = aka::mat3f::identity();
	transform *= aka::mat3f::translate(position);
	transform *= aka::mat3f::translate(0.5f * size);
	transform *= aka::mat3f::rotate(m_rotation);
	transform *= aka::mat3f::translate(-0.5f * size);
	transform *= aka::mat3f::scale(size);

	UniformBuffer* data = static_cast<UniformBuffer*>(device->map(m_ubo, gfx::BufferMap::Write));
	data->model = mat4f::rotate(vec3f(0, 1, 0), m_rotation);// mat4f::from2D(transform);
	data->view = mat4f::lookAtView(point3f(3), point3f(0));
	data->proj = mat4f::perspective(anglef::degree(60.f), width() / (float)height(), 0.1f, 100.f);
	device->unmap(m_ubo);

	gfx::CommandList* cmd = device->getGraphicCommandList(_frame);
	//cmd->bindIndexBuffer(indexBuffer, gfx::IndexFormat::UnsignedInt);
	cmd->bindVertexBuffer(0, m_vertexBuffer);
	cmd->bindPipeline(m_pipeline);
	cmd->bindDescriptorSet(0, m_set);
	cmd->beginRenderPass(m_renderPass, device->get(m_backbuffer, _frame), gfx::ClearState{ gfx::ClearMask::All, {1.f, 1.f, 1.f, 1.f}, 1.f, 1 });
	cmd->draw(36, 0, 1);
	cmd->endRenderPass();
}