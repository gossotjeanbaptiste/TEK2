package name.modid.worldgen;

import com.mojang.serialization.MapCodec;

import net.minecraft.core.BlockPos;
import net.minecraft.world.level.LevelReader;
import net.minecraft.world.level.block.BrewingStandBlock;
import net.minecraft.world.level.block.Blocks;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructurePlaceSettings;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructureProcessor;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructureProcessorType;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructureTemplate;

import org.jspecify.annotations.Nullable;

import name.modid.blocks.ModBlocks;

public class EndBrewingStandProcessor extends StructureProcessor {
	public static final MapCodec<EndBrewingStandProcessor> CODEC = MapCodec.unit(() -> EndBrewingStandProcessor.INSTANCE);
	public static final EndBrewingStandProcessor INSTANCE = new EndBrewingStandProcessor();

	private EndBrewingStandProcessor() {
	}

	@Override
	public StructureTemplate.@Nullable StructureBlockInfo processBlock(
		LevelReader level,
		BlockPos targetPosition,
		BlockPos referencePos,
		StructureTemplate.StructureBlockInfo originalBlockInfo,
		StructureTemplate.StructureBlockInfo processedBlockInfo,
		StructurePlaceSettings settings
	) {
		if (processedBlockInfo == null || !processedBlockInfo.state().is(Blocks.BREWING_STAND)) {
			return processedBlockInfo;
		}

		BlockState oldState = processedBlockInfo.state();
		BlockState newState = ModBlocks.END_BREWING_STAND.defaultBlockState();
		for (int i = 0; i < BrewingStandBlock.HAS_BOTTLE.length; i++) {
			newState = newState.setValue(BrewingStandBlock.HAS_BOTTLE[i], oldState.getValue(BrewingStandBlock.HAS_BOTTLE[i]));
		}

		return new StructureTemplate.StructureBlockInfo(processedBlockInfo.pos(), newState, processedBlockInfo.nbt());
	}

	@Override
	protected StructureProcessorType<?> getType() {
		return ModStructureProcessors.END_BREWING_STAND_SWAP;
	}
}
