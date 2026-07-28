package name.modid.entity;

import net.fabricmc.fabric.api.event.lifecycle.v1.ServerTickEvents;

import net.minecraft.core.BlockPos;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.server.level.ServerPlayer;
import net.minecraft.util.RandomSource;
import net.minecraft.world.Difficulty;
import net.minecraft.world.entity.EntitySpawnReason;
import net.minecraft.world.entity.SpawnGroupData;
import net.minecraft.world.entity.SpawnPlacementTypes;
import net.minecraft.world.entity.monster.Monster;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.gamerules.GameRules;
import net.minecraft.world.level.levelgen.Heightmap;
import net.minecraft.world.phys.AABB;
import net.minecraft.world.phys.Vec3;

/**
 * Vanilla's natural spawner picks a uniformly random Y between the world's min height and a
 * column's surface height before searching for a spot there (see NaturalSpawner#getRandomPosWithin).
 * On the End's thin floating islands that heavily biases spawns towards low altitudes, since most
 * of that Y range is just void under a high island. This runs its own attempt straight against each
 * column's real surface, so the Watchling spawns evenly regardless of how high an island sits.
 */
public final class WatchlingSpawner {
	private static final int ATTEMPT_INTERVAL_TICKS = 200;
	private static final int ATTEMPTS_PER_PLAYER = 3;
	private static final double MIN_DISTANCE = 28.0;
	private static final double MAX_DISTANCE = 48.0;
	private static final int MIN_GROUP_SIZE = 3;
	private static final int MAX_GROUP_SIZE = 6;
	private static final double GROUP_JITTER = 4.0;
	private static final double NEARBY_CAP_RADIUS = 48.0;
	private static final int NEARBY_CAP = 4;

	private WatchlingSpawner() {
	}

	public static void init() {
		ServerTickEvents.END_LEVEL_TICK.register(WatchlingSpawner::onLevelTick);
	}

	private static void onLevelTick(ServerLevel level) {
		if (level.dimension() != Level.END
			|| level.getDifficulty() == Difficulty.PEACEFUL
			|| !level.getGameRules().get(GameRules.SPAWN_MONSTERS)
			|| level.getGameTime() % ATTEMPT_INTERVAL_TICKS != 0) {
			return;
		}

		for (ServerPlayer player : level.players()) {
			if (!player.isSpectator()) {
				trySpawnNear(level, player, level.getRandom());
			}
		}
	}

	private static void trySpawnNear(ServerLevel level, ServerPlayer player, RandomSource random) {
		for (int i = 0; i < ATTEMPTS_PER_PLAYER; i++) {
			double angle = random.nextDouble() * (Math.PI * 2.0);
			double distance = MIN_DISTANCE + random.nextDouble() * (MAX_DISTANCE - MIN_DISTANCE);
			int anchorX = player.getBlockX() + (int) Math.round(Math.cos(angle) * distance);
			int anchorZ = player.getBlockZ() + (int) Math.round(Math.sin(angle) * distance);
			int anchorY = level.getHeight(Heightmap.Types.MOTION_BLOCKING_NO_LEAVES, anchorX, anchorZ);
			BlockPos anchorPos = new BlockPos(anchorX, anchorY, anchorZ);

			double distanceSqr = player.distanceToSqr(anchorX + 0.5, anchorY, anchorZ + 0.5);
			if (distanceSqr < MIN_DISTANCE * MIN_DISTANCE || distanceSqr > MAX_DISTANCE * MAX_DISTANCE) {
				continue;
			}
			if (!isValidSpawn(level, anchorPos, random)) {
				continue;
			}

			AABB nearby = AABB.ofSize(Vec3.atCenterOf(anchorPos), NEARBY_CAP_RADIUS * 2, NEARBY_CAP_RADIUS * 2, NEARBY_CAP_RADIUS * 2);
			if (level.getEntities(ModEntities.WATCHLING, nearby, entity -> true).size() >= NEARBY_CAP) {
				continue;
			}

			spawnGroup(level, anchorPos, random);
			return;
		}
	}

	private static void spawnGroup(ServerLevel level, BlockPos anchorPos, RandomSource random) {
		int groupSize = MIN_GROUP_SIZE + random.nextInt(MAX_GROUP_SIZE - MIN_GROUP_SIZE + 1);
		SpawnGroupData groupData = null;

		for (int member = 0; member < groupSize; member++) {
			BlockPos pos;
			if (member == 0) {
				pos = anchorPos;
			} else {
				int x = anchorPos.getX() + (int) Math.round((random.nextDouble() - 0.5) * 2.0 * GROUP_JITTER);
				int z = anchorPos.getZ() + (int) Math.round((random.nextDouble() - 0.5) * 2.0 * GROUP_JITTER);
				int y = level.getHeight(Heightmap.Types.MOTION_BLOCKING_NO_LEAVES, x, z);
				pos = new BlockPos(x, y, z);
				if (!isValidSpawn(level, pos, random)) {
					continue;
				}
			}

			WatchlingEntity watchling = ModEntities.WATCHLING.create(level, EntitySpawnReason.NATURAL);
			if (watchling == null) {
				continue;
			}
			watchling.snapTo(pos.getX() + 0.5, pos.getY(), pos.getZ() + 0.5, random.nextFloat() * 360.0F, 0.0F);
			groupData = watchling.finalizeSpawn(level, level.getCurrentDifficultyAt(pos), EntitySpawnReason.NATURAL, groupData);
			level.addFreshEntity(watchling);
		}
	}

	private static boolean isValidSpawn(ServerLevel level, BlockPos pos, RandomSource random) {
		if (!SpawnPlacementTypes.ON_GROUND.isSpawnPositionOk(level, pos, ModEntities.WATCHLING)) {
			return false;
		}
		if (!Monster.checkMonsterSpawnRules(ModEntities.WATCHLING, level, EntitySpawnReason.NATURAL, pos, random)) {
			return false;
		}
		return level.noCollision(ModEntities.WATCHLING.getSpawnAABB(pos.getX() + 0.5, pos.getY(), pos.getZ() + 0.5));
	}
}
