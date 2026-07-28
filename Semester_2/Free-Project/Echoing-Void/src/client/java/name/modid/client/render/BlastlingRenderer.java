package name.modid.client.render;

import net.minecraft.client.model.geom.ModelLayerLocation;
import net.minecraft.client.renderer.entity.EntityRendererProvider;
import net.minecraft.client.renderer.entity.HumanoidMobRenderer;
import net.minecraft.resources.Identifier;

import name.modid.EchoingVoid;
import name.modid.entity.BlastlingEntity;

public class BlastlingRenderer extends HumanoidMobRenderer<BlastlingEntity, BlastlingRenderState, BlastlingModel> {
	public static final ModelLayerLocation LAYER = new ModelLayerLocation(EchoingVoid.id("blastling"), "main");
	public static final ModelLayerLocation FLAME_LAYER = new ModelLayerLocation(EchoingVoid.id("blastling"), "flame");
	private static final Identifier TEXTURE = EchoingVoid.id("textures/mob/blastling/blastling.png");
	private static final int FLAME_CYCLE = 32;

	public BlastlingRenderer(EntityRendererProvider.Context context) {
		super(context, new BlastlingModel(context.bakeLayer(LAYER)), 0.5F);
		this.addLayer(new BlastlingEyesLayer(this));
		this.addLayer(new BlastlingFlameLayer(this, new BlastlingFlameModel(context.bakeLayer(FLAME_LAYER))));
	}

	@Override
	public BlastlingRenderState createRenderState() {
		return new BlastlingRenderState();
	}

	@Override
	public void extractRenderState(BlastlingEntity entity, BlastlingRenderState state, float partialTick) {
		super.extractRenderState(entity, state, partialTick);
		state.teleportHidden = entity.isTeleportHidden();
		// Staggered per-entity like Watchling's blink phase, so a whole group doesn't flare in lockstep.
		state.flameFrame = (entity.tickCount + entity.getId() * 5) % FLAME_CYCLE;
	}

	@Override
	public Identifier getTextureLocation(BlastlingRenderState state) {
		return TEXTURE;
	}
}
