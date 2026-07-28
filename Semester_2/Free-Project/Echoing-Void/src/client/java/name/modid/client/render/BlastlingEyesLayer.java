package name.modid.client.render;

import com.mojang.blaze3d.vertex.PoseStack;

import net.minecraft.client.renderer.SubmitNodeCollector;
import net.minecraft.client.renderer.entity.RenderLayerParent;
import net.minecraft.client.renderer.entity.layers.EyesLayer;
import net.minecraft.client.renderer.rendertype.RenderType;
import net.minecraft.client.renderer.rendertype.RenderTypes;
import net.minecraft.client.renderer.texture.OverlayTexture;

import name.modid.EchoingVoid;

/**
 * Same full-bright eyes trick as {@link WatchlingEyesLayer}: reuses BlastlingModel's own geometry
 * with an alternate texture (blastling_eye.png, transparent except at the eye pixels on the head's
 * UV). No blink states here - only one eye texture was provided, unlike Watchling's open/half pair.
 */
public class BlastlingEyesLayer extends EyesLayer<BlastlingRenderState, BlastlingModel> {
	private static final RenderType EYES = RenderTypes.eyes(EchoingVoid.id("textures/mob/blastling/blastling_eye.png"));

	public BlastlingEyesLayer(RenderLayerParent<BlastlingRenderState, BlastlingModel> parent) {
		super(parent);
	}

	@Override
	public void submit(PoseStack poseStack, SubmitNodeCollector collector, int packedLight, BlastlingRenderState state, float yRot, float xRot) {
		if (state.teleportHidden) {
			return;
		}
		collector.order(1).submitModel(this.getParentModel(), state, poseStack, EYES, packedLight, OverlayTexture.NO_OVERLAY, state.outlineColor, null);
	}

	@Override
	public RenderType renderType() {
		return EYES;
	}
}
