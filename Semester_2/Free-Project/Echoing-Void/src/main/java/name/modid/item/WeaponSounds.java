package name.modid.item;

import java.util.Map;

import net.minecraft.sounds.SoundEvent;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;

import name.modid.sound.ModSounds;

// Equip/pick up/drop identity sounds shared by the Echoing Void unique weapons.
public class WeaponSounds {
	private record Sounds(SoundEvent equip, SoundEvent pickUp, SoundEvent drop) {
	}

	private static final Map<Item, Sounds> BY_ITEM = Map.of(
		ModItems.THE_BEGINNING_OF_THE_END, new Sounds(
			ModSounds.THE_BEGINNING_OF_THE_END_EQUIP,
			ModSounds.THE_BEGINNING_OF_THE_END_PICK_UP,
			ModSounds.THE_BEGINNING_OF_THE_END_DROP),
		ModItems.CALL_OF_THE_VOID, new Sounds(
			ModSounds.CALL_OF_THE_VOID_EQUIP,
			ModSounds.CALL_OF_THE_VOID_PICK_UP,
			ModSounds.CALL_OF_THE_VOID_DROP));

	public static SoundEvent equip(ItemStack stack) {
		Sounds sounds = BY_ITEM.get(stack.getItem());
		return sounds == null ? null : sounds.equip();
	}

	public static SoundEvent pickUp(ItemStack stack) {
		Sounds sounds = BY_ITEM.get(stack.getItem());
		return sounds == null ? null : sounds.pickUp();
	}

	public static SoundEvent drop(ItemStack stack) {
		Sounds sounds = BY_ITEM.get(stack.getItem());
		return sounds == null ? null : sounds.drop();
	}
}
