package name.modid.item;

import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.entity.projectile.Projectile;
import net.minecraft.world.entity.projectile.arrow.AbstractArrow;
import net.minecraft.world.item.BowItem;
import net.minecraft.world.item.Item;

import name.modid.sound.ModSounds;

public class CallOfTheVoidBowItem extends BowItem {
	// getPowerForTime() maxes out at 1.0F for a full draw, and BowItem.releaseUsing() passes
	// power * 3.0F down as the "power" argument here - so 3.0F means a fully charged shot.
	public static final float FULL_CHARGE_POWER = 3.0F;
	public static final String CHARGED_SHOT_TAG = "echoing_void:call_of_the_void_charged_shot";

	public CallOfTheVoidBowItem(Item.Properties properties) {
		super(properties);
	}

	@Override
	protected void shootProjectile(LivingEntity shooter, Projectile projectile, int index, float power, float uncertainty, float angle, LivingEntity targetOverride) {
		super.shootProjectile(shooter, projectile, index, power, uncertainty, angle, targetOverride);

		boolean fullyCharged = power >= FULL_CHARGE_POWER - 0.01F;
		if (fullyCharged && projectile instanceof AbstractArrow arrow) {
			arrow.addTag(CHARGED_SHOT_TAG);
		}

		shooter.level().playSound(null, shooter.getX(), shooter.getY(), shooter.getZ(),
			fullyCharged ? ModSounds.CALL_OF_THE_VOID_CHARGED_SHOT : ModSounds.CALL_OF_THE_VOID_SHOT,
			shooter.getSoundSource(), 1.0F, 1.0F);
	}
}
