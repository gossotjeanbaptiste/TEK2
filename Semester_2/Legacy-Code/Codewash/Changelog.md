# Launch

In the file `package.json`, the version of `vite` has gone from `npm:rolldown-vite@7.2.5` to `^5.0.0`. This as permitted the project to be launched with the `npm run dev` command, which was not possible before.  

# Themes 

Add the `div` : `<div className="h-64 overflow-y-auto scrollbar-thin scrollbar-thumb-slate-600 scrollbar-track-slate-800/30 pr-2">` at project/src/components/drawers/CosmeticsDrawer.jsx:70 to the themes lists to actually be visible.
Add some other themes to the list, like the Spy Theme, the Fantasy Theme and the SciFi Theme.

# Player

Add keyboard detection when updating the player username: 
- Escape key to close the input field
- Enter key to validate the new username
Add a callback on the function `onCreatePlayer` which is called when the player is created or updated.

# Decipher game (Memory repetition games)

Add visual feedback when hovering and clicking an axolotl.
Visual Feedback when hovering : Axolotl seems to come closer
Visual Feedback when Clicking : Axolotl seems to come more closer than hovering and a yellow ring is appearing around the clicked axolotl