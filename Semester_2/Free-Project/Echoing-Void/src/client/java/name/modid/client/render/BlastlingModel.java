package name.modid.client.render;

import net.minecraft.client.model.HumanoidModel;
import net.minecraft.client.model.geom.PartPose;
import net.minecraft.client.model.geom.builders.CubeListBuilder;
import net.minecraft.client.model.geom.builders.LayerDefinition;
import net.minecraft.client.model.geom.builders.MeshDefinition;
import net.minecraft.client.model.geom.builders.PartDefinition;
import net.minecraft.client.model.geom.ModelPart;

/**
 * Same head/torso/thin-leg proportions as {@link WatchlingModel} (Blastling reads as Watchling's
 * sibling, not a vanilla humanoid), with slightly bulkier forearms for the Dungeons "large hands"
 * look, capped at 2 blocks tall. The "hat" part is left empty like Watchling's - the animated head
 * flame is a separate {@link BlastlingFlameModel} rendered by {@link BlastlingFlameLayer}, not part
 * of this model's geometry/texture.
 */
public class BlastlingModel extends HumanoidModel<BlastlingRenderState> {
	public BlastlingModel(ModelPart root) {
		super(root);
	}

	public static LayerDefinition createBodyLayer() {
		MeshDefinition mesh = new MeshDefinition();
		PartDefinition root = mesh.getRoot();

		PartDefinition head = root.addOrReplaceChild("head",
			CubeListBuilder.create()
				.texOffs(0, 0)
				.addBox(-4.0F, -8.0F, -4.0F, 8.0F, 8.0F, 8.0F),
			PartPose.offset(0.0F, 0.0F, 0.0F));
		head.addOrReplaceChild("hat", CubeListBuilder.create(), PartPose.ZERO);

		root.addOrReplaceChild("body",
			CubeListBuilder.create()
				.texOffs(16, 16)
				.addBox(-5.0F, 0.0F, -2.0F, 10.0F, 12.0F, 4.0F),
			PartPose.offset(0.0F, 0.0F, 0.0F));

		root.addOrReplaceChild("right_arm",
			CubeListBuilder.create()
				.texOffs(0, 32)
				.addBox(-2.5F, -2.0F, -2.5F, 5.0F, 18.0F, 5.0F),
			PartPose.offset(-6.0F, 2.0F, 0.0F));

		root.addOrReplaceChild("left_arm",
			CubeListBuilder.create()
				.texOffs(21, 32)
				.addBox(-2.5F, -2.0F, -2.5F, 5.0F, 18.0F, 5.0F),
			PartPose.offset(6.0F, 2.0F, 0.0F));

		root.addOrReplaceChild("right_leg",
			CubeListBuilder.create()
				.texOffs(42, 32)
				.addBox(-1.0F, 0.0F, -1.0F, 2.0F, 12.0F, 2.0F),
			PartPose.offset(-1.8F, 12.0F, 0.0F));

		root.addOrReplaceChild("left_leg",
			CubeListBuilder.create()
				.texOffs(50, 32)
				.addBox(-1.0F, 0.0F, -1.0F, 2.0F, 12.0F, 2.0F),
			PartPose.offset(1.8F, 12.0F, 0.0F));

		return LayerDefinition.create(mesh, 64, 64);
	}
}
