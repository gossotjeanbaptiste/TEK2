package name.modid.client.render;

import com.mojang.blaze3d.vertex.PoseStack;

import net.minecraft.client.renderer.SubmitNodeCollector;
import net.minecraft.client.renderer.entity.RenderLayerParent;
import net.minecraft.client.renderer.entity.layers.RenderLayer;
import net.minecraft.client.renderer.rendertype.RenderType;
import net.minecraft.client.renderer.rendertype.RenderTypes;
import net.minecraft.client.renderer.texture.OverlayTexture;

import name.modid.EchoingVoid;

/**
 * Animates the flame flaring around (not replacing) the Blastling's head. Renders its own small
 * {@link BlastlingFlameModel} rather than reusing BlastlingModel's geometry (see that class for
 * why), so this can't extend EyesLayer like WatchlingEyesLayer does - it's a plain RenderLayer
 * that submits a second, independent model each frame.
 */
public class BlastlingFlameLayer extends RenderLayer<BlastlingRenderState, BlastlingModel> {
	private static final int FRAME_COUNT = 32;
	private static final RenderType[] FRAMES = buildFrames();

	private static RenderType[] buildFrames() {
		RenderType[] frames = new RenderType[FRAME_COUNT];
		for (int i = 0; i < FRAME_COUNT; i++) {
			// Assets are 1-indexed: flame_01.png .. flame_32.png
			frames[i] = RenderTypes.eyes(EchoingVoid.id(String.format("textures/mob/blastling/flame/flame_%02d.png", i + 1)));
		}
		return frames;
	}

	private final BlastlingFlameModel flameModel;

	public BlastlingFlameLayer(RenderLayerParent<BlastlingRenderState, BlastlingModel> parent, BlastlingFlameModel flameModel) {
		super(parent);
		this.flameModel = flameModel;
	}

	@Override
	public void submit(PoseStack poseStack, SubmitNodeCollector collector, int packedLight, BlastlingRenderState state, float yRot, float xRot) {
		if (state.teleportHidden) {
			return;
		}
		this.flameModel.setupAnim(state);
		RenderType type = FRAMES[Math.floorMod(state.flameFrame, FRAME_COUNT)];
		collector.order(1).submitModel(this.flameModel, state, poseStack, type, packedLight, OverlayTexture.NO_OVERLAY, state.outlineColor, null);
	}
}
