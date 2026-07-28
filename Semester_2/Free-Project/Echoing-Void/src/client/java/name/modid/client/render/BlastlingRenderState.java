package name.modid.client.render;

import net.minecraft.client.renderer.entity.state.HumanoidRenderState;

public class BlastlingRenderState extends HumanoidRenderState {
	public boolean teleportHidden;
	/** 0-31: current frame of the head flame animation. */
	public int flameFrame;
}
