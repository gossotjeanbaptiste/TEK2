package name.modid.client.render;

import net.minecraft.client.model.EntityModel;
import net.minecraft.client.model.geom.ModelPart;
import net.minecraft.client.model.geom.PartPose;
import net.minecraft.client.model.geom.builders.CubeListBuilder;
import net.minecraft.client.model.geom.builders.LayerDefinition;
import net.minecraft.client.model.geom.builders.MeshDefinition;
import net.minecraft.client.model.geom.builders.PartDefinition;

/**
 * Standalone tiny model for the head-flame overlay. Deliberately NOT a child part of
 * {@link BlastlingModel} - {@code submitModel} bakes UVs against whatever pixel size the
 * LayerDefinition declares, so sharing BlastlingModel's 64x64 canvas would force every flame
 * frame texture to also be 64x64. Keeping it a separate model with its own 8x8 canvas means each
 * frame texture can just be a plain 8x8 image.
 *
 * <p>Three fixed flares glued flush against the head (left, right, back) - each rotated so its
 * broad face points outward from the head instead of forward/backward.
 */
public class BlastlingFlameModel extends EntityModel<BlastlingRenderState> {
	public BlastlingFlameModel(ModelPart root) {
		super(root);
	}

	// Box is 8x8 (w x h) - same face size as the head box (addBox(-4,-8,-4, 8,8,8)) - and 1 thick,
	// centered on its own local origin so rotating it in place doesn't need extra offset
	// compensation. UV unwrap for these dimensions (2*d+2*w wide, d+h tall) is 18x9.
	private static CubeListBuilder flareShape() {
		return CubeListBuilder.create()
			.texOffs(0, 0)
			.addBox(-4.0F, -4.0F, -0.5F, 8.0F, 8.0F, 1.0F);
	}

	public static LayerDefinition createBodyLayer() {
		MeshDefinition mesh = new MeshDefinition();
		PartDefinition root = mesh.getRoot();

		// Head box is addBox(-4,-8,-4, 8,8,8): left face at x=-4, right face at x=4, back face at
		// z=4, spanning y=-8..0. Rotating +-90deg around Y turns the flare's broad face to point
		// sideways instead of forward/back, so it reads as attached to the head rather than a flat
		// card floating beside it. The back flare needs no rotation - its own front/back faces
		// already point along Z. y=-4.0 centers each flare on the head's own y=-8..0 span.
		root.addOrReplaceChild("flame_left", flareShape(),
			PartPose.offsetAndRotation(-4.5F, -4.0F, 0.0F, 0.0F, (float) (Math.PI / 2.0), 0.0F));
		root.addOrReplaceChild("flame_right", flareShape(),
			PartPose.offsetAndRotation(4.5F, -4.0F, 0.0F, 0.0F, (float) (-Math.PI / 2.0), 0.0F));
		root.addOrReplaceChild("flame_back", flareShape(),
			PartPose.offset(0.0F, -4.0F, 4.5F));

		return LayerDefinition.create(mesh, 18, 9);
	}

	@Override
	public void setupAnim(BlastlingRenderState state) {
		// Fixed flares glued to the head - the baked pose is final, nothing to animate per frame.
	}
}
