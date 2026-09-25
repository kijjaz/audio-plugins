import React, { useRef, useState, useCallback, useEffect } from 'react';
import { SliderProps } from '../types';

const Slider: React.FC<SliderProps> = ({ 
  label, 
  value, 
  min, 
  max, 
  onChange, 
  displayValue,
  unit 
}) => {
  const [isDragging, setIsDragging] = useState(false);
  const trackRef = useRef<HTMLDivElement>(null);

  const percentage = Math.min(100, Math.max(0, ((value - min) / (max - min)) * 100));

  const updateValueFromClientX = useCallback((clientX: number) => {
    if (!trackRef.current) return;
    const rect = trackRef.current.getBoundingClientRect();
    const x = Math.max(0, Math.min(clientX - rect.left, rect.width));
    const percent = x / rect.width;
    const newValue = min + (percent * (max - min));
    onChange(newValue);
  }, [max, min, onChange]);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    updateValueFromClientX(e.clientX);
  };

  const handleTouchStart = (e: React.TouchEvent) => {
    setIsDragging(true);
    updateValueFromClientX(e.touches[0].clientX);
  };

  const handleMouseMove = useCallback((e: MouseEvent) => {
    if (isDragging) updateValueFromClientX(e.clientX);
  }, [isDragging, updateValueFromClientX]);

  const handleTouchMove = useCallback((e: TouchEvent) => {
    if (isDragging) updateValueFromClientX(e.touches[0].clientX);
  }, [isDragging, updateValueFromClientX]);

  const handleMouseUp = useCallback(() => {
    setIsDragging(false);
  }, []);

  useEffect(() => {
    if (isDragging) {
      window.addEventListener('mousemove', handleMouseMove);
      window.addEventListener('touchmove', handleTouchMove);
      window.addEventListener('mouseup', handleMouseUp);
      window.addEventListener('touchend', handleMouseUp);
    }
    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('touchmove', handleTouchMove);
      window.removeEventListener('mouseup', handleMouseUp);
      window.removeEventListener('touchend', handleMouseUp);
    };
  }, [isDragging, handleMouseMove, handleTouchMove, handleMouseUp]);

  return (
    <div className="w-full space-y-3 select-none touch-none">
      <div className="flex justify-between items-center">
        <span className="text-[10px] uppercase font-bold text-white/40 tracking-widest">{label}</span>
        {displayValue ? (
            <span className="font-serif italic text-primary text-sm">{displayValue}</span>
        ) : (
            <span className="font-serif italic text-primary text-sm">
                {Math.round(value)}{unit || ''}
            </span>
        )}
      </div>
      
      <div 
        ref={trackRef}
        className="relative h-4 flex items-center cursor-pointer group py-2" // Added py-2 for larger hit area
        onMouseDown={handleMouseDown}
        onTouchStart={handleTouchStart}
      >
        {/* Track Background */}
        <div className="w-full h-1 bg-black/60 rounded-full overflow-hidden border border-white/5"></div>
        
        {/* Filled Track */}
        <div 
          className="absolute left-0 h-1 bg-primary shadow-[0_0_8px_#c5a463] rounded-full pointer-events-none"
          style={{ width: `${percentage}%` }}
        ></div>
        
        {/* Thumb */}
        <div 
          className="absolute top-1/2 -translate-y-1/2 size-4 bg-primary border-4 border-carbon rounded-full shadow-xl transition-transform duration-100 group-hover:scale-110 pointer-events-none"
          style={{ left: `${percentage}%`, transform: `translate(-50%, -50%) ${isDragging ? 'scale(1.2)' : ''}` }}
        ></div>
      </div>
    </div>
  );
};

export default Slider;
