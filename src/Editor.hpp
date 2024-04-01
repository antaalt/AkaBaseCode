#pragma once

#include <Aka/Core/Application.h>

class Editor : public aka::Application
{
public:
	Editor(const aka::Config& cfg);
	~Editor();
	void onCreate(int argc, char* argv[]) override;
	void onDestroy() override;
	void onFixedUpdate(aka::Time time) override;
	void onUpdate(aka::Time time) override;
	void onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame) override;
private:
	aka::gfx::BufferHandle m_ubo = aka::gfx::BufferHandle::null;
	aka::gfx::BufferHandle m_vertexBuffer = aka::gfx::BufferHandle::null;
	aka::gfx::ShaderHandle m_vertex = aka::gfx::ShaderHandle::null;
	aka::gfx::ShaderHandle m_fragment = aka::gfx::ShaderHandle::null;
	aka::gfx::ProgramHandle m_program = aka::gfx::ProgramHandle::null;
	aka::gfx::GraphicPipelineHandle m_pipeline = aka::gfx::GraphicPipelineHandle::null;
	aka::gfx::DescriptorPoolHandle m_pool = aka::gfx::DescriptorPoolHandle::null;
	aka::gfx::DescriptorSetHandle m_set = aka::gfx::DescriptorSetHandle::null;
	aka::gfx::RenderPassHandle m_renderPass = aka::gfx::RenderPassHandle::null;
	aka::gfx::BackbufferHandle m_backbuffer = aka::gfx::BackbufferHandle::null;
	aka::anglef m_rotation = aka::anglef::radian(0.f);
};