package name.modid.client.render;

import net.minecraft.client.renderer.entity.state.HumanoidRenderState;

public class WatchlingRenderState extends HumanoidRenderState {
	public boolean teleportHidden;
	/** 0 = fully open, 1 = half-closed (closing/opening), 2 = fully closed. */
	public int blinkStage;
}
