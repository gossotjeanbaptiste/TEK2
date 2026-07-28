package name.modid.client.render;

import net.minecraft.client.model.geom.ModelLayerLocation;
import net.minecraft.client.renderer.entity.EntityRendererProvider;
import net.minecraft.client.renderer.entity.HumanoidMobRenderer;
import net.minecraft.resources.Identifier;

import name.modid.EchoingVoid;
import name.modid.entity.WatchlingEntity;

public class WatchlingRenderer extends HumanoidMobRenderer<WatchlingEntity, WatchlingRenderState, WatchlingModel> {
	public static final ModelLayerLocation LAYER = new ModelLayerLocation(EchoingVoid.id("watchling"), "main");
	private static final Identifier TEXTURE = EchoingVoid.id("textures/mob/watchling/watchling.png");

	public WatchlingRenderer(EntityRendererProvider.Context context) {
		super(context, new WatchlingModel(context.bakeLayer(LAYER)), 0.5F);
		this.addLayer(new WatchlingEyesLayer(this));
	}

	@Override
	public WatchlingRenderState createRenderState() {
		return new WatchlingRenderState();
	}

	@Override
	protected boolean isBodyVisible(WatchlingRenderState state) {
		return super.isBodyVisible(state);
	}

	@Override
	public void extractRenderState(WatchlingEntity entity, WatchlingRenderState state, float partialTick) {
		super.extractRenderState(entity, state, partialTick);
		// Watchlings blink more often than other mobs, given how many eyes they have.
		// Stagger the cycle per-entity so a whole group doesn't blink in lockstep, and step
		// through half-closed -> closed -> half-closed instead of a hard on/off cut.
		int cycle = 90;
		int phase = (entity.tickCount + entity.getId() * 37) % cycle;
		if (phase < 2) {
			state.blinkStage = 1;
		} else if (phase < 4) {
			state.blinkStage = 2;
		} else if (phase < 6) {
			state.blinkStage = 1;
		} else {
			state.blinkStage = 0;
		}
	}

	@Override
	public Identifier getTextureLocation(WatchlingRenderState state) {
		return TEXTURE;
	}
}
