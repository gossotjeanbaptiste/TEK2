package name.modid.client.render;

import com.mojang.blaze3d.vertex.PoseStack;

import net.minecraft.client.renderer.SubmitNodeCollector;
import net.minecraft.client.renderer.entity.RenderLayerParent;
import net.minecraft.client.renderer.entity.layers.EyesLayer;
import net.minecraft.client.renderer.rendertype.RenderType;
import net.minecraft.client.renderer.rendertype.RenderTypes;
import net.minecraft.client.renderer.texture.OverlayTexture;

import name.modid.EchoingVoid;

public class WatchlingEyesLayer extends EyesLayer<WatchlingRenderState, WatchlingModel> {
	private static final RenderType EYES_OPEN = RenderTypes.eyes(EchoingVoid.id("textures/mob/watchling/watchling_eyes.png"));
	private static final RenderType EYES_HALF = RenderTypes.eyes(EchoingVoid.id("textures/mob/watchling/watchling_eyes_half.png"));

	public WatchlingEyesLayer(RenderLayerParent<WatchlingRenderState, WatchlingModel> parent) {
		super(parent);
	}

	@Override
	public void submit(PoseStack poseStack, SubmitNodeCollector collector, int packedLight, WatchlingRenderState state, float yRot, float xRot) {
		// Only skip the eyes during the teleport-hide window (fully unseen). A real Invisibility
		// potion should still leave the eyes showing, classic Enderman-style.
		if (state.teleportHidden || state.blinkStage == 2) {
			return;
		}
		RenderType type = state.blinkStage == 1 ? EYES_HALF : EYES_OPEN;
		collector.order(1).submitModel(this.getParentModel(), state, poseStack, type, packedLight, OverlayTexture.NO_OVERLAY, state.outlineColor, null);
	}

	@Override
	public RenderType renderType() {
		return EYES_OPEN;
	}
}
