package name.modid.block.entity;

import net.fabricmc.fabric.api.object.builder.v1.block.entity.FabricBlockEntityTypeBuilder;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.block.entity.BlockEntityType;

import name.modid.EchoingVoid;
import name.modid.blocks.ModBlocks;

public class ModBlockEntities {
	public static final BlockEntityType<EndBrewingStandBlockEntity> END_BREWING_STAND = register("end_brewing_stand",
		FabricBlockEntityTypeBuilder.create(EndBrewingStandBlockEntity::new, ModBlocks.END_BREWING_STAND).build());

	private static <T extends net.minecraft.world.level.block.entity.BlockEntity> BlockEntityType<T> register(String name, BlockEntityType<T> type) {
		ResourceKey<BlockEntityType<?>> key = ResourceKey.create(Registries.BLOCK_ENTITY_TYPE, EchoingVoid.id(name));
		return Registry.register(BuiltInRegistries.BLOCK_ENTITY_TYPE, key, type);
	}

	public static void init() {
	}
}
