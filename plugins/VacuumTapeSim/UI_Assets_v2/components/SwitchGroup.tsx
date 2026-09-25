import React from 'react';
import { SwitchGroupProps } from '../types';

const SwitchGroup: React.FC<SwitchGroupProps> = ({ options, value, onChange }) => {
  return (
    <div className="grid grid-cols-3 gap-1 bg-black/40 p-1 rounded-lg border border-white/5">
      {options.map((option) => {
        const isActive = value === option.value;
        return (
          <button
            key={String(option.value)}
            onClick={() => onChange(option.value)}
            className={`
              py-2 text-[10px] font-black uppercase tracking-widest rounded-md transition-all duration-200
              ${isActive 
                ? 'text-primary bg-primary/10 border border-primary/30 shadow-[0_0_15px_rgba(197,164,99,0.1)]' 
                : 'text-white/30 hover:text-white/50 border border-transparent'
              }
            `}
          >
            {option.label}
          </button>
        );
      })}
    </div>
  );
};

export default SwitchGroup;
