export interface KnobProps {
  label: string;
  value: number;
  min: number;
  max: number;
  onChange: (value: number) => void;
  size?: 'sm' | 'md' | 'lg' | 'xl';
  icon?: string;
  rotationRange?: number; // Total degrees of rotation (e.g., 270)
}

export interface SliderProps {
  label: string;
  value: number;
  min: number;
  max: number;
  onChange: (value: number) => void;
  displayValue?: string;
  unit?: string;
}

export interface SwitchOption {
  label: string;
  value: string | number;
}

export interface SwitchGroupProps {
  options: SwitchOption[];
  value: string | number;
  onChange: (value: any) => void;
}
