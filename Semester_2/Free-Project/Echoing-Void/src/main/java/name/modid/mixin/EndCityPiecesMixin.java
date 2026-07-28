package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.world.level.block.Rotation;
import net.minecraft.world.level.levelgen.structure.structures.EndCityPieces;
import net.minecraft.world.level.levelgen.structure.templatesystem.StructurePlaceSettings;

import name.modid.worldgen.EndBrewingStandProcessor;

@Mixin(EndCityPieces.EndCityPiece.class)
public class EndCityPiecesMixin {
	@Inject(method = "makeSettings", at = @At("RETURN"), cancellable = true)
	private static void echoingVoid$addEndBrewingStandProcessor(boolean overwrite, Rotation rotation, CallbackInfoReturnable<StructurePlaceSettings> cir) {
		cir.getReturnValue().addProcessor(EndBrewingStandProcessor.INSTANCE);
	}
}
