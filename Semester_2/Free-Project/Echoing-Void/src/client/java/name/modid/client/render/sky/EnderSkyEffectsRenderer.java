package name.modid.client.render.sky;

import java.util.OptionalDouble;
import java.util.OptionalInt;

import org.joml.Matrix4f;
import org.joml.Vector3f;
import org.joml.Vector4f;

import com.mojang.blaze3d.buffers.GpuBuffer;
import com.mojang.blaze3d.buffers.GpuBufferSlice;
import com.mojang.blaze3d.pipeline.RenderPipeline;
import com.mojang.blaze3d.pipeline.RenderTarget;
import com.mojang.blaze3d.systems.RenderPass;
import com.mojang.blaze3d.systems.RenderSystem;
import com.mojang.blaze3d.textures.GpuSampler;
import com.mojang.blaze3d.textures.GpuTextureView;
import com.mojang.blaze3d.vertex.BufferBuilder;
import com.mojang.blaze3d.vertex.ByteBufferBuilder;
import com.mojang.blaze3d.vertex.MeshData;
import com.mojang.blaze3d.vertex.VertexFormat.IndexType;
import com.mojang.blaze3d.systems.RenderSystem.AutoStorageIndexBuffer;

import net.minecraft.client.Minecraft;
import net.minecraft.client.renderer.texture.AbstractTexture;
import net.minecraft.resources.Identifier;

import name.modid.EchoingVoid;

public final class EnderSkyEffectsRenderer {
	private static final Identifier NOISE_TEXTURE = EchoingVoid.id("textures/effect/ender_sky_noise.png");
	private static final float EXTENT = 100.0F;

	public static void render() {
		RenderPipeline pipeline = EnderSkyEffectsRenderPipelines.ENDER_SKY_EFFECTS;

		Matrix4f modelViewMatrix = new Matrix4f(RenderSystem.getModelViewMatrix());
		modelViewMatrix.setTranslation(0.0F, 0.0F, 0.0F);
		GpuBufferSlice dynamicTransforms = RenderSystem.getDynamicUniforms()
			.writeTransform(modelViewMatrix, new Vector4f(1.0F, 1.0F, 1.0F, 1.0F), new Vector3f(), new Matrix4f());

		ByteBufferBuilder byteBufferBuilder = new ByteBufferBuilder(pipeline.getVertexFormat().getVertexSize() * 24);
		MeshData meshData;
		try {
			BufferBuilder builder = new BufferBuilder(byteBufferBuilder, pipeline.getVertexFormatMode(), pipeline.getVertexFormat());
			addSkyCube(builder);
			meshData = builder.buildOrThrow();
		} catch (Throwable throwable) {
			byteBufferBuilder.close();
			throw throwable;
		}

		draw(pipeline, meshData, dynamicTransforms);
	}

	private static void addSkyCube(BufferBuilder builder) {
		float e = EXTENT;

		// -Y (down)
		builder.addVertex(-e, -e, -e);
		builder.addVertex(-e, -e, e);
		builder.addVertex(e, -e, e);
		builder.addVertex(e, -e, -e);

		// +Y (up)
		builder.addVertex(-e, e, -e);
		builder.addVertex(e, e, -e);
		builder.addVertex(e, e, e);
		builder.addVertex(-e, e, e);

		// -Z (north)
		builder.addVertex(-e, -e, -e);
		builder.addVertex(e, -e, -e);
		builder.addVertex(e, e, -e);
		builder.addVertex(-e, e, -e);

		// +Z (south)
		builder.addVertex(-e, -e, e);
		builder.addVertex(-e, e, e);
		builder.addVertex(e, e, e);
		builder.addVertex(e, -e, e);

		// -X (west)
		builder.addVertex(-e, -e, -e);
		builder.addVertex(-e, e, -e);
		builder.addVertex(-e, e, e);
		builder.addVertex(-e, -e, e);

		// +X (east)
		builder.addVertex(e, -e, -e);
		builder.addVertex(e, -e, e);
		builder.addVertex(e, e, e);
		builder.addVertex(e, e, -e);
	}

	private static void draw(RenderPipeline pipeline, MeshData meshData, GpuBufferSlice dynamicTransforms) {
		try {
			// Resolve (and lazily upload, if this is the first access) the noise texture before
			// opening the render pass below - GpuDevice forbids texture uploads while a pass is open.
			AbstractTexture noiseTexture = Minecraft.getInstance().getTextureManager().getTexture(NOISE_TEXTURE);
			GpuTextureView noiseTextureView = noiseTexture.getTextureView();
			GpuSampler noiseSampler = noiseTexture.getSampler();

			GpuBuffer vertexBuffer = pipeline.getVertexFormat().uploadImmediateVertexBuffer(meshData.vertexBuffer());
			AutoStorageIndexBuffer sequentialBuffer = RenderSystem.getSequentialBuffer(meshData.drawState().mode());
			GpuBuffer indexBuffer = sequentialBuffer.getBuffer(meshData.drawState().indexCount());
			IndexType indexType = sequentialBuffer.type();

			RenderTarget renderTarget = Minecraft.getInstance().getMainRenderTarget();
			GpuTextureView colorTexture = RenderSystem.outputColorTextureOverride != null
				? RenderSystem.outputColorTextureOverride
				: renderTarget.getColorTextureView();
			GpuTextureView depthTexture = renderTarget.useDepth
				? (RenderSystem.outputDepthTextureOverride != null ? RenderSystem.outputDepthTextureOverride : renderTarget.getDepthTextureView())
				: null;

			RenderPass renderPass = RenderSystem.getDevice()
				.createCommandEncoder()
				.createRenderPass(() -> "Echoing Void ender sky effects", colorTexture, OptionalInt.empty(), depthTexture, OptionalDouble.empty());

			try {
				renderPass.setPipeline(pipeline);
				renderPass.setVertexBuffer(0, vertexBuffer);
				renderPass.setIndexBuffer(indexBuffer, indexType);

				RenderSystem.bindDefaultUniforms(renderPass);
				renderPass.setUniform("DynamicTransforms", dynamicTransforms);
				renderPass.bindTexture("Sampler0", noiseTextureView, noiseSampler);

				renderPass.drawIndexed(0, 0, meshData.drawState().indexCount(), 1);
			} finally {
				renderPass.close();
			}
		} finally {
			meshData.close();
		}
	}

	private EnderSkyEffectsRenderer() {
	}
}
