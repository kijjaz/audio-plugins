import React from 'react';

const NavBar: React.FC = () => {
  const items = [
    { label: 'Console', icon: 'tune', active: true },
    { label: 'Presets', icon: 'history_edu', active: false },
    { label: 'Lab', icon: 'stethoscope', active: false },
    { label: 'Archive', icon: 'verified', active: false },
  ];

  return (
    <nav className="fixed bottom-0 inset-x-0 bg-[#0a0a09]/95 backdrop-blur-xl border-t border-white/5 px-6 py-4 flex items-center justify-around z-50 safe-area-pb">
      {items.map((item) => (
        <button 
          key={item.label}
          className={`flex flex-col items-center gap-1.5 transition-colors duration-200 ${
            item.active ? 'text-primary' : 'text-white/30 hover:text-white/60'
          }`}
        >
          <span className="material-symbols-outlined text-2xl">{item.icon}</span>
          <span className="text-[8px] uppercase font-black tracking-widest">{item.label}</span>
        </button>
      ))}
    </nav>
  );
};

export default NavBar;
