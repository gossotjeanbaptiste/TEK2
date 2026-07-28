package name.modid.worldgen;

import com.mojang.serialization.MapCodec;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructureProcessor;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructureProcessorType;

import name.modid.EchoingVoid;

public class ModStructureProcessors {
	public static final StructureProcessorType<EndBrewingStandProcessor> END_BREWING_STAND_SWAP =
		register("end_brewing_stand_swap", EndBrewingStandProcessor.CODEC);

	private static <P extends StructureProcessor> StructureProcessorType<P> register(String name, MapCodec<P> codec) {
		ResourceKey<StructureProcessorType<?>> key = ResourceKey.create(Registries.STRUCTURE_PROCESSOR, EchoingVoid.id(name));
		return Registry.register(BuiltInRegistries.STRUCTURE_PROCESSOR, key, () -> codec);
	}

	public static void init() {
	}
}
