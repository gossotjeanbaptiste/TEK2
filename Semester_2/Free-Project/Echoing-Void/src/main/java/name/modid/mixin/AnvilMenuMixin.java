package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Redirect;

import net.minecraft.core.Holder;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.inventory.AnvilMenu;
import net.minecraft.world.item.enchantment.Enchantment;

import name.modid.item.ModItems;

// Removes the vanilla 40-level "Too Expensive!" anvil cap entirely (normally bypassed only for
// creative-mode players via Player.hasInfiniteMaterials()) - legendary gear like Call of the Void
// can easily stack past it (innate Void Strike I plus Punch/Infinity/Flame/etc.), and there's no
// reason survival players should be blocked from combining further past that point either.
@Mixin(AnvilMenu.class)
public abstract class AnvilMenuMixin {
	@Redirect(method = "createResult", at = @At(value = "INVOKE", target = "Lnet/minecraft/world/entity/player/Player;hasInfiniteMaterials()Z"))
	private boolean echoingVoid$noTooExpensive(Player player) {
		return true;
	}

	// Call of the Void specifically ignores exclusive-set conflicts (e.g. Infinity + Mending,
	// normally mutually exclusive on any bow) so any enchantment combo can be stacked onto it.
	@Redirect(method = "createResult", at = @At(value = "INVOKE",
		target = "Lnet/minecraft/world/item/enchantment/Enchantment;areCompatible(Lnet/minecraft/core/Holder;Lnet/minecraft/core/Holder;)Z"))
	private boolean echoingVoid$compatibleOnCallOfTheVoid(Holder<Enchantment> first, Holder<Enchantment> second) {
		AnvilMenu self = (AnvilMenu) (Object) this;
		if (self.getSlot(AnvilMenu.INPUT_SLOT).getItem().is(ModItems.CALL_OF_THE_VOID)) {
			return true;
		}
		return Enchantment.areCompatible(first, second);
	}
}
