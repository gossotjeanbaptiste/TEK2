package name.modid.item;

import java.util.Map;
import java.util.WeakHashMap;

import net.minecraft.sounds.SoundEvent;
import net.minecraft.world.entity.player.Player;

import name.modid.sound.ModSounds;

// Mirrors the "three quick slashes then a spin" identity of the Dungeons dual daggers: every
// third consecutive hit within the combo window plays the combo sound instead of a plain attack.
public class TheBeginningOfTheEndCombo {
	private static final int HITS_PER_COMBO = 3;
	private static final int COMBO_WINDOW_TICKS = 30;

	private static final Map<Player, Streak> STREAKS = new WeakHashMap<>();

	private record Streak(int hits, long lastHitGameTime) {
	}

	public static void onHit(Player player) {
		long now = player.level().getGameTime();
		Streak previous = STREAKS.get(player);
		int hits = (previous != null && now - previous.lastHitGameTime() <= COMBO_WINDOW_TICKS) ? previous.hits() + 1 : 1;

		boolean comboFinisher = hits >= HITS_PER_COMBO;
		SoundEvent sound = comboFinisher ? ModSounds.THE_BEGINNING_OF_THE_END_COMBO : ModSounds.THE_BEGINNING_OF_THE_END_ATTACK;
		player.level().playSound(null, player.getX(), player.getY(), player.getZ(),
			sound, player.getSoundSource(), 1.0F, 1.0F);

		STREAKS.put(player, new Streak(comboFinisher ? 0 : hits, now));
	}
}
