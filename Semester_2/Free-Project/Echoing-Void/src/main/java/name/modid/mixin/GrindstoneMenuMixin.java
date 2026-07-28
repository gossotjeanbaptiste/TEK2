package name.modid.mixin;

import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.core.Holder;
import net.minecraft.core.registries.Registries;
import net.minecraft.world.inventory.ContainerLevelAccess;
import net.minecraft.world.inventory.GrindstoneMenu;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.enchantment.Enchantment;
import net.minecraft.world.item.enchantment.EnchantmentHelper;

import name.modid.enchantment.ModEnchantments;
import name.modid.item.ModItems;

// The Echoing Void unique weapons carry an innate Void Strike I that isn't a curse (so it can
// still be freely upgraded via anvil), but a grindstone pass should never strip it below I.
@Mixin(GrindstoneMenu.class)
public class GrindstoneMenuMixin {
	@Shadow @Final private ContainerLevelAccess access;

	@Inject(method = "removeNonCursesFrom", at = @At("RETURN"))
	private void echoingVoid$restoreInnateVoidStrike(ItemStack stack, CallbackInfoReturnable<ItemStack> cir) {
		if (!stack.is(ModItems.THE_BEGINNING_OF_THE_END) && !stack.is(ModItems.CALL_OF_THE_VOID)) {
			return;
		}
		this.access.execute((level, pos) -> {
			Holder<Enchantment> voidStrike = level.registryAccess().lookupOrThrow(Registries.ENCHANTMENT).getOrThrow(ModEnchantments.VOID_STRIKE);
			EnchantmentHelper.updateEnchantments(stack, mutable -> mutable.set(voidStrike, 1));
		});
	}
}
