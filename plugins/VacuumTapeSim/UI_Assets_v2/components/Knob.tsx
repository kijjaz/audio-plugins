import React, { useState, useEffect, useRef, useCallback } from 'react';
import { KnobProps } from '../types';

const Knob: React.FC<KnobProps> = ({ 
  label, 
  value, 
  min, 
  max, 
  onChange, 
  size = 'md', 
  icon,
  rotationRange = 270 
}) => {
  const [isDragging, setIsDragging] = useState(false);
  const startYRef = useRef<number>(0);
  const startValueRef = useRef<number>(0);
  const knobRef = useRef<HTMLDivElement>(null);

  // Map value to rotation degrees
  // -135deg to +135deg for 270deg range
  const normalizedValue = (value - min) / (max - min);
  const rotation = (normalizedValue * rotationRange) - (rotationRange / 2);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    startYRef.current = e.clientY;
    startValueRef.current = value;
    document.body.style.cursor = 'ns-resize';
  };

  const handleTouchStart = (e: React.TouchEvent) => {
    setIsDragging(true);
    startYRef.current = e.touches[0].clientY;
    startValueRef.current = value;
    document.body.style.cursor = 'ns-resize';
  };

  const handleMove = useCallback((clientY: number) => {
    if (!isDragging) return;
    
    const deltaY = startYRef.current - clientY;
    const sensitivity = 200; // Pixels to move full range
    const deltaValue = (deltaY / sensitivity) * (max - min);
    
    let newValue = startValueRef.current + deltaValue;
    newValue = Math.max(min, Math.min(max, newValue));
    
    onChange(newValue);
  }, [isDragging, max, min, onChange]);

  const handleMouseMove = useCallback((e: MouseEvent) => {
    handleMove(e.clientY);
  }, [handleMove]);

  const handleTouchMove = useCallback((e: TouchEvent) => {
    handleMove(e.touches[0].clientY);
  }, [handleMove]);

  const handleEnd = useCallback(() => {
    setIsDragging(false);
    document.body.style.cursor = 'default';
  }, []);

  useEffect(() => {
    if (isDragging) {
      window.addEventListener('mousemove', handleMouseMove);
      window.addEventListener('mouseup', handleEnd);
      window.addEventListener('touchmove', handleTouchMove);
      window.addEventListener('touchend', handleEnd);
    } else {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleEnd);
      window.removeEventListener('touchmove', handleTouchMove);
      window.removeEventListener('touchend', handleEnd);
    }
    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleEnd);
      window.removeEventListener('touchmove', handleTouchMove);
      window.removeEventListener('touchend', handleEnd);
    };
  }, [isDragging, handleMouseMove, handleTouchMove, handleEnd]);

  // Size classes
  const sizeClasses = {
    sm: 'w-16 h-16',
    md: 'w-20 h-20',
    lg: 'w-28 h-28',
    xl: 'w-36 h-36'
  };

  const indicatorSize = {
    sm: 'h-2 w-0.5 top-2',
    md: 'h-3 w-1 top-2',
    lg: 'h-4 w-1 top-3',
    xl: 'h-6 w-1.5 top-3'
  };

  return (
    <div className="flex flex-col items-center gap-3 select-none touch-none">
      <div 
        ref={knobRef}
        className={`relative ${sizeClasses[size]} rounded-full cursor-ns-resize group`}
        onMouseDown={handleMouseDown}
        onTouchStart={handleTouchStart}
        role="slider"
        aria-valuenow={value}
        aria-valuemin={min}
        aria-valuemax={max}
        aria-label={label}
        style={{
          background: 'radial-gradient(circle at 35% 35%, #5a4b2d 0%, #1e1b14 70%, #0a0a09 100%)',
          boxShadow: `
            inset 0 2px 4px rgba(255,255,255,0.1),
            inset 0 -2px 10px rgba(0,0,0,0.8),
            0 10px 20px rgba(0,0,0,0.6)
          `
        }}
      >
        {/* Outer Ring Effect */}
        <div className="absolute inset-0 rounded-full border border-white/10 pointer-events-none"></div>
        
        {/* Rotating Face Container */}
        <div 
          className="absolute inset-0 w-full h-full pointer-events-none"
          style={{ transform: `rotate(${rotation}deg)` }}
        >
             {/* Indicator Tick */}
            <div className={`absolute left-1/2 -translate-x-1/2 bg-primary rounded-full shadow-[0_0_5px_#c5a463] ${indicatorSize[size]}`}></div>

            {/* Rotating Icon */}
            {icon && (
                <div 
                    className="absolute inset-0 flex items-center justify-center text-primary transition-opacity duration-100"
                    style={{ 
                        opacity: 0.3 + (normalizedValue * 0.7), // Icon glows brighter as value increases
                        textShadow: `0 0 ${normalizedValue * 15}px rgba(197, 164, 99, 0.6)`
                    }}
                >
                    <span className="material-symbols-outlined text-4xl">{icon}</span>
                </div>
            )}
        </div>

        {/* Shine/Reflection Overlay */}
        <div className="absolute inset-0 rounded-full opacity-30 pointer-events-none bg-gradient-to-br from-white/10 to-transparent"></div>
      </div>
      
      {/* Label */}
      <div className="text-center space-y-1">
        <label className="text-[9px] font-black uppercase tracking-[0.2em] text-white/50 block">
          {label}
        </label>
        {isDragging && (
             <div className="text-[10px] text-primary font-mono tabular-nums animate-pulse">
                 {value.toFixed(1)}
             </div>
        )}
      </div>
    </div>
  );
};

export default Knob;