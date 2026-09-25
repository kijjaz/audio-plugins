import React, { useState } from 'react';
import TapeVisualizer from './components/TapeVisualizer';
import Knob from './components/Knob';
import Slider from './components/Slider';
import SwitchGroup from './components/SwitchGroup';
import NavBar from './components/NavBar';

const App: React.FC = () => {
  // State for controls
  const [threshold, setThreshold] = useState(75);
  const [drain, setDrain] = useState(50);
  const [recovery, setRecovery] = useState(25);
  const [compTime, setCompTime] = useState(25);
  
  const [drive, setDrive] = useState(60);
  const [hysteresis, setHysteresis] = useState(72);
  const [ips, setIps] = useState<string>('15');
  
  const [wow, setWow] = useState(12);
  const [flutter, setFlutter] = useState(8);
  
  const [oversample, setOversample] = useState<string>('4x');
  const [masterVolume, setMasterVolume] = useState(85);

  return (
    <div className="min-h-screen pb-32 font-display bg-[#0a0a09] text-white selection:bg-primary/30 selection:text-white">
      {/* Header */}
      <header className="flex items-center p-6 pb-2 justify-between sticky top-0 z-40 bg-[#0a0a09]/90 backdrop-blur-sm">
        <button className="text-primary hover:text-white transition-colors">
            <span className="material-symbols-outlined text-2xl">menu</span>
        </button>
        <h1 className="font-serif text-lg font-black tracking-[0.3em] uppercase text-primary/90 text-center flex-1">
          Vacuum Tape Sim Pro
        </h1>
        <button className="text-primary hover:text-white transition-colors">
            <span className="material-symbols-outlined text-2xl">settings</span>
        </button>
      </header>

      <main className="max-w-md mx-auto p-4 space-y-8 animate-fade-in">
        
        {/* Visualizer Section */}
        <section>
          <TapeVisualizer ips={ips} />
        </section>

        {/* Compressor Section */}
        <section className="space-y-6">
          <div className="flex items-end justify-between border-b border-primary/20 pb-2">
            <h2 className="font-serif italic text-primary text-xl font-bold tracking-tight">Compressor</h2>
            <div className="font-mono border-2 border-primary/40 text-primary/40 px-2 py-0.5 rounded uppercase font-bold text-[8px] rotate-[-5deg] mix-blend-overlay">
              Laboratory Tested 1952
            </div>
          </div>
          
          <div className="grid grid-cols-3 gap-4">
            <Knob 
              label="Threshold" 
              value={threshold} 
              min={0} 
              max={100} 
              onChange={setThreshold} 
            />
            <Knob 
              label="Drain" 
              value={drain} 
              min={0} 
              max={100} 
              onChange={setDrain} 
            />
            <Knob 
              label="Recovery" 
              value={recovery} 
              min={0} 
              max={100} 
              onChange={setRecovery} 
            />
          </div>

          <div className="bg-carbon rounded-xl border border-white/5 p-5 shadow-[inset_0_2px_15px_rgba(0,0,0,0.5)]">
            <Slider 
              label="Comp Time"
              value={compTime}
              min={1}
              max={100}
              onChange={setCompTime}
              unit=" ms"
            />
          </div>
        </section>

        {/* Tape Drive Section */}
        <section className="space-y-6">
          <div className="flex items-end justify-between border-b border-primary/20 pb-2">
            <h2 className="font-serif italic text-primary text-xl font-bold tracking-tight">Tape Drive</h2>
            <div className="text-[10px] text-primary/40 uppercase font-mono tracking-tighter">Audio Research Archive</div>
          </div>

          <div className="bg-carbon rounded-xl border border-white/5 p-8 flex flex-col items-center gap-8 shadow-[inset_0_2px_15px_rgba(0,0,0,0.8)] relative overflow-hidden">
             {/* Subtle Texture */}
             <div className="absolute inset-0 opacity-10 bg-[url('https://www.transparenttextures.com/patterns/carbon-fibre.png')]"></div>

            <div className="flex flex-col items-center gap-4 z-10">
              <div className="p-2 border-[4px] border-primary/20 rounded-full">
                <Knob 
                    label="" 
                    value={drive} 
                    min={0} 
                    max={100} 
                    onChange={setDrive} 
                    size="xl"
                    icon="bolt"
                />
              </div>
              <div className="bg-primary/10 border border-primary/30 px-6 py-1 rounded-full">
                <span className="text-xs font-black text-primary uppercase tracking-[0.3em]">Drive</span>
              </div>
            </div>

            <div className="w-full space-y-8 z-10">
              <Slider 
                label="Hysteresis"
                value={hysteresis}
                min={0}
                max={100}
                onChange={setHysteresis}
                unit="%"
              />
              
              <SwitchGroup 
                value={ips}
                onChange={setIps}
                options={[
                  { label: '9.5 IPS', value: '9.5' },
                  { label: '15 IPS', value: '15' },
                  { label: '30 IPS', value: '30' },
                ]}
              />
            </div>
          </div>
        </section>

        {/* Bottom Split Section */}
        <div className="grid grid-cols-2 gap-4">
          
          {/* Modulation */}
          <section className="space-y-4">
            <h3 className="font-serif italic text-primary text-lg font-bold border-b border-primary/20 pb-1">Modulation</h3>
            <div className="bg-carbon rounded-xl border border-white/5 p-4 space-y-6 shadow-[inset_0_2px_15px_rgba(0,0,0,0.5)] h-full">
              <Slider 
                label="Wow"
                value={wow}
                min={0}
                max={50}
                onChange={setWow}
                unit="%"
                displayValue={`${wow}%`}
              />
              <Slider 
                label="Flutter"
                value={flutter}
                min={0}
                max={50}
                onChange={setFlutter}
                unit="%"
                displayValue={`${flutter.toString().padStart(2, '0')}%`}
              />
            </div>
          </section>

          {/* Master */}
          <section className="space-y-4">
            <h3 className="font-serif italic text-primary text-lg font-bold border-b border-primary/20 pb-1">Master</h3>
            <div className="bg-carbon rounded-xl border border-white/5 p-4 space-y-6 shadow-[inset_0_2px_15px_rgba(0,0,0,0.5)] h-full">
               
               {/* Oversample Selector */}
               <div className="space-y-2">
                 <label className="text-[8px] font-black uppercase tracking-widest text-white/40 block">Oversample</label>
                 <div className="flex justify-between text-[9px] font-mono text-primary/60 px-1">
                   {['1x', '4x', '8x'].map(opt => (
                     <button
                        key={opt}
                        onClick={() => setOversample(opt)}
                        className={`hover:text-primary transition-colors ${oversample === opt ? 'text-primary font-bold underline decoration-primary decoration-2 underline-offset-4' : ''}`}
                     >
                       {opt}
                     </button>
                   ))}
                 </div>
               </div>

               <div className="pt-1">
                 <Slider 
                    label="Volume"
                    value={masterVolume}
                    min={0}
                    max={100}
                    onChange={setMasterVolume}
                    displayValue=" "
                 />
               </div>
            </div>
          </section>

        </div>
      </main>

      <NavBar />
    </div>
  );
};

export default App;