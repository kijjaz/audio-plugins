import React, { useMemo } from 'react';

interface TapeVisualizerProps {
  ips: string;
}

const TapeVisualizer: React.FC<TapeVisualizerProps> = ({ ips }) => {
  // Calculate animation duration based on IPS speed
  // Higher IPS = Faster speed = Lower duration
  const animationDuration = useMemo(() => {
    switch(ips) {
      case '30': return '2s';
      case '15': return '4s';
      case '9.5': return '6s';
      default: return '4s';
    }
  }, [ips]);

  const reelStyle = {
    animationDuration: animationDuration
  };

  return (
    <div className="relative w-full aspect-[16/9] bg-carbon rounded-xl border border-white/5 overflow-hidden shadow-[inset_0_2px_15px_rgba(0,0,0,0.8)]">
      {/* Background Texture Overlay */}
      <div className="absolute inset-0 opacity-20 bg-[url('https://www.transparenttextures.com/patterns/carbon-fibre.png')] mix-blend-overlay"></div>
      
      {/* Dark Overlay/Vignette */}
      <div className="absolute inset-0 bg-radial-gradient from-transparent to-black/60"></div>

      <div className="relative h-full flex items-center justify-around px-8">
        {/* Left Reel */}
        <div 
          className="relative w-24 h-24 sm:w-32 sm:h-32 animate-spin-slow"
          style={reelStyle}
        >
          <div className="absolute inset-0 rounded-full border-4 border-white/10 flex items-center justify-center bg-black/20 backdrop-blur-sm">
            <div className="w-full h-1 bg-white/10 rotate-45"></div>
            <div className="w-full h-1 bg-white/10 -rotate-45"></div>
            <div className="w-4 h-4 rounded-full bg-primary/20 border border-primary/40"></div>
          </div>
          {/* Tape on reel visual */}
          <div className="absolute inset-3 rounded-full border-[6px] border-[#3a3a3a] opacity-80"></div>
        </div>

        {/* Center Tube Assembly */}
        <div className="relative flex flex-col items-center z-10">
          {/* Glass Tube */}
          <div className="w-16 sm:w-20 h-28 sm:h-36 bg-gradient-to-b from-white/5 to-white/0 rounded-t-full border border-white/10 flex flex-col items-center justify-end pb-6 relative overflow-hidden backdrop-blur-sm">
            {/* Inner Glow Core */}
            <div className="w-1.5 h-20 bg-gradient-to-t from-primary via-white to-primary rounded-full blur-[1px] shadow-[0_0_25px_#c5a463] animate-pulse"></div>
            
            {/* Top Reflection */}
            <div className="absolute top-4 left-4 w-6 h-6 bg-white/10 rounded-full blur-md"></div>
          </div>
          
          {/* Base */}
          <div className="w-24 h-8 bg-[#2D2D2D] rounded-b-md border-t border-white/20 flex justify-around items-center px-6 shadow-lg">
             {/* Status LEDs */}
            <div className="w-1.5 h-1.5 rounded-full bg-primary shadow-[0_0_5px_#c5a463]"></div>
            <div className="w-1.5 h-1.5 rounded-full bg-primary shadow-[0_0_5px_#c5a463] opacity-80"></div>
          </div>
        </div>

        {/* Right Reel */}
        <div 
          className="relative w-24 h-24 sm:w-32 sm:h-32 animate-spin-reverse-slow"
          style={reelStyle}
        >
          <div className="absolute inset-0 rounded-full border-4 border-white/10 flex items-center justify-center bg-black/20 backdrop-blur-sm">
            <div className="w-full h-1 bg-white/10 rotate-45"></div>
            <div className="w-full h-1 bg-white/10 -rotate-45"></div>
            <div className="w-4 h-4 rounded-full bg-primary/20 border border-primary/40"></div>
          </div>
          <div className="absolute inset-5 rounded-full border-[4px] border-[#3a3a3a] opacity-80"></div>
        </div>
      </div>

      {/* VU Meters (Simulated at bottom left) */}
      <div className="absolute left-6 bottom-6 flex gap-3">
        {[0.7, 0.5].map((level, i) => (
          <div key={i} className="w-16 h-1.5 bg-black/60 rounded-full overflow-hidden border border-white/5">
            <div 
                className="h-full bg-primary shadow-[0_0_10px_#c5a463] transition-all duration-100 ease-out"
                style={{ width: `${level * 100}%` }}
            ></div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default TapeVisualizer;