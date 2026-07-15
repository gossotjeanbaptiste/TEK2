import React from 'react';
import { Users, Music, Trash2, AlertTriangle, Globe, Plus } from 'lucide-react';
import ProfileCard from '../profile/ProfileCard';

const CreatePlayerButton = ({ profileId, profileName, onCreateClick }) => (
    <div 
        onClick={onCreateClick}
        className="relative w-full h-[100px] rounded-xl overflow-hidden transition-all cursor-pointer group select-none border-4 border-dashed border-blue-400/50 hover:border-blue-300 bg-slate-900/30 hover:bg-slate-800/50 flex items-center justify-center hover:scale-[1.02] shadow-lg"
    >
        <div className="absolute inset-0 bg-gradient-to-br from-blue-500/10 to-purple-500/10 opacity-0 group-hover:opacity-100 transition-opacity"></div>
        <div className="relative flex flex-col items-center gap-2 text-center">
            <div className="flex items-center gap-2">
                <Plus className="text-blue-400 animate-pulse" size={32} />
            </div>
            <div>
                <h4 className="text-lg font-bold text-blue-300 uppercase tracking-wider">Create Player</h4>
                <p className="text-xs text-slate-400 mt-1">Click to create the profile of the {profileName}</p>
            </div>
        </div>
    </div>
);

const SettingsDrawer = ({ isOpen, onReset, bgmVol, setBgmVol, sfxVol, setSfxVol, currentProfile, onSwitchProfile, profileNames, onRenameProfile, getProfileStats, parentStatus, onParentVerified, currentSkills, onCreatePlayer }) => (
    <div 
        className={`fixed h-full w-[85%] md:w-[60%] bg-[#0f172a]/95 backdrop-blur-xl z-50 border-r-4 border-slate-700 shadow-2xl transition-transform duration-300 ease-in-out ${isOpen ? 'translate-x-0' : '-translate-x-full'}`}
        style={{ top: 0, left: 0 }}
    >
        <div className="p-6 h-full flex flex-col text-slate-200 font-sans">
            {/* Header - Fixed at top */}
            <div className="flex justify-between items-center border-b-2 border-slate-700 pb-4 shrink-0">
                <h2 className="text-4xl text-yellow-400 font-bold uppercase tracking-widest drop-shadow-md" style={{ fontFamily: '"VT323", monospace' }}>Settings</h2>
            </div>
            
            {/* Scrollable content area */}
            <div className="flex-1 overflow-y-auto scrollbar-hide py-6">
                <div className="flex flex-col gap-6">
                    <div>
                        <h3 className="text-xl text-blue-300 mb-5 font-bold flex items-center gap-3 uppercase tracking-wider"><Users size={20} /> Select User</h3>
                        <div className="flex flex-col gap-4">
                            {[1, 2, 3].map(id => {
                                const stats = id === currentProfile ? getProfileStats(id, currentSkills) : getProfileStats(id);
                                const isCreated = stats && stats.theme;
                                
                                return isCreated ? (
                                    <ProfileCard 
                                        key={id} 
                                        id={id} 
                                        name={profileNames[id]} 
                                        stats={stats} 
                                        isCurrent={currentProfile === id} 
                                        onSwitch={onSwitchProfile} 
                                        onRename={onRenameProfile} 
                                        isParent={parentStatus && parentStatus[id]} 
                                        onParentVerified={onParentVerified} 
                                    />
                                ) : (
                                    <CreatePlayerButton
                                        key={id}
                                        profileId={id}
                                        profileName={profileNames[id]}
                                        onCreateClick={() => {
                                            onSwitchProfile(id);
                                            if (onCreatePlayer) {
                                                onCreatePlayer(id);
                                            }
                                        }}
                                    />
                                );
                            })}
                        </div>
                    </div>
                    
                    <div>
                        <h3 className="text-xl text-blue-300 mb-5 font-bold flex items-center gap-3 uppercase tracking-wider"><Music size={20} /> Audio Configuration</h3>
                        <div className="space-y-6 bg-slate-900/50 p-5 rounded-xl border-2 border-slate-600">
                            <div className="px-3">
                                <div className="flex justify-between mb-3 text-slate-400 font-bold text-sm uppercase"><span className="pl-2">Music Volume</span><span className="text-yellow-400">{Math.round(bgmVol * 100)}%</span></div>
                                <input type="range" min="0" max="1" step="0.01" value={bgmVol} onChange={(e) => setBgmVol(parseFloat(e.target.value))} className="w-full" />
                            </div>
                            <div className="px-3">
                                <div className="flex justify-between mb-3 text-slate-400 font-bold text-sm uppercase"><span className="pl-2">SFX Volume</span><span className="text-yellow-400">{Math.round(sfxVol * 100)}%</span></div>
                                <input type="range" min="0" max="1" step="0.01" value={sfxVol} onChange={(e) => setSfxVol(parseFloat(e.target.value))} className="w-full" />
                            </div>
                        </div>
                    </div>
                    
                    <div>
                        <h3 className="text-xl text-blue-300 mb-5 font-bold flex items-center gap-3 uppercase tracking-wider"><Globe size={20} /> Traduction</h3>
                        <div className="space-y-6 bg-slate-900/50 p-5 rounded-xl border-2 border-slate-600">
                            {/* Language selection here. Maybe deepl or something else */}
                            <p className="text-slate-400 text-sm">Don't ask too much! For now, be happy that later you will have traduction.</p>
                        </div>
                    </div>
                </div>
            </div>
            
            {/* Danger Zone - Fixed at bottom */}
            <div className="shrink-0 pt-4 border-t-2 border-red-900/50">
                <h3 className="text-xl text-red-400 mb-4 font-bold flex items-center gap-3 uppercase tracking-wider">
                    <AlertTriangle size={20} className="text-red-500" /> Danger Zone
                </h3>
                <button onClick={onReset} className="w-full bg-red-950/50 hover:bg-red-900/80 text-red-400 p-3 rounded-lg border border-red-900/50 hover:border-red-500 font-bold text-lg flex items-center justify-center gap-3 transition-all">
                    <Trash2 size={20} /> DELETE PROFILE PROGRESS
                </button>
            </div>
        </div>
    </div>
);

export default SettingsDrawer;
