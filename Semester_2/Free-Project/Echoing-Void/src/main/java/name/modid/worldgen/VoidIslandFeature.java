package name.modid.worldgen;

import com.mojang.serialization.Codec;

import net.minecraft.core.BlockPos;
import net.minecraft.util.Mth;
import net.minecraft.util.RandomSource;
import net.minecraft.world.level.WorldGenLevel;
import net.minecraft.world.level.levelgen.feature.Feature;
import net.minecraft.world.level.levelgen.feature.FeaturePlaceContext;
import net.minecraft.world.level.levelgen.feature.configurations.NoneFeatureConfiguration;

import name.modid.blocks.ModBlocks;

// Same shrinking-radius-stack shape as vanilla's EndIslandFeature (the feature behind Small End
// Islands), just scaled down to a ~3-6 block footprint and only 1-2 layers thick so islands read
// as tight parkour platforms rather than full landmasses, and built from Void Stone instead of
// End Stone.
public class VoidIslandFeature extends Feature<NoneFeatureConfiguration> {
	public VoidIslandFeature(Codec<NoneFeatureConfiguration> codec) {
		super(codec);
	}

	@Override
	public boolean place(FeaturePlaceContext<NoneFeatureConfiguration> context) {
		WorldGenLevel level = context.level();
		RandomSource random = context.random();
		BlockPos origin = context.origin();
		float size = 1.0F + random.nextFloat() * 1.5F;

		for (int y = 0; size > 0.5F; y--) {
			for (int x = Mth.floor(-size); x <= Mth.ceil(size); x++) {
				for (int z = Mth.floor(-size); z <= Mth.ceil(size); z++) {
					if (x * x + z * z <= (size + 1.0F) * (size + 1.0F)) {
						this.setBlock(level, origin.offset(x, y, z), ModBlocks.VOID_STONE.defaultBlockState());
					}
				}
			}

			size -= 1.0F + random.nextFloat();
		}

		return true;
	}
}
