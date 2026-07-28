package name.modid.fluid;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.material.Fluid;
import net.minecraft.world.level.material.FlowingFluid;

import name.modid.EchoingVoid;

public class ModFluids {
	public static final FlowingFluid VOID_LIQUID = register("void_liquid", new VoidLiquidFluid.Source());
	public static final FlowingFluid FLOWING_VOID_LIQUID = register("flowing_void_liquid", new VoidLiquidFluid.Flowing());

	private static <T extends FlowingFluid> T register(String name, T fluid) {
		ResourceKey<Fluid> key = ResourceKey.create(Registries.FLUID, EchoingVoid.id(name));
		return Registry.register(BuiltInRegistries.FLUID, key, fluid);
	}

	public static void init() {
	}
}
