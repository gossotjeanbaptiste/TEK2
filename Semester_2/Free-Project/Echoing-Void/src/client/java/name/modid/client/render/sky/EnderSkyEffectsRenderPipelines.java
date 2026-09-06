package name.modid.client.render.sky;

import com.mojang.blaze3d.pipeline.BlendFunction;
import com.mojang.blaze3d.pipeline.ColorTargetState;
import com.mojang.blaze3d.pipeline.RenderPipeline;
import com.mojang.blaze3d.shaders.UniformType;
import com.mojang.blaze3d.vertex.DefaultVertexFormat;
import com.mojang.blaze3d.vertex.VertexFormat.Mode;

import name.modid.EchoingVoid;

public final class EnderSkyEffectsRenderPipelines {
	public static final RenderPipeline ENDER_SKY_EFFECTS = RenderPipeline.builder()
		.withLocation(EchoingVoid.id("pipeline/ender_sky_effects"))
		.withVertexShader(EchoingVoid.id("core/ender_sky_effects"))
		.withFragmentShader(EchoingVoid.id("core/ender_sky_effects"))
		.withUniform("DynamicTransforms", UniformType.UNIFORM_BUFFER)
		.withUniform("Projection", UniformType.UNIFORM_BUFFER)
		.withUniform("Globals", UniformType.UNIFORM_BUFFER)
		.withSampler("Sampler0")
		.withVertexFormat(DefaultVertexFormat.POSITION, Mode.QUADS)
		.withColorTargetState(new ColorTargetState(BlendFunction.ADDITIVE))
		.withCull(false)
		.build();

	static {
		// Iris intercepts every RenderPipeline it recognizes for shaderpack compatibility, even
		// with no pack selected; without this, it silently drops draws using an unknown pipeline.
		IrisCompat.assignSkyTexturedPipeline(ENDER_SKY_EFFECTS);
	}

	private EnderSkyEffectsRenderPipelines() {
	}
}
