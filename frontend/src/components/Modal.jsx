import React, { useEffect } from 'react';
import { X } from 'lucide-react';

export default function Modal({ isOpen, onClose, title, children, maxWidth = '580px' }) {
  useEffect(() => {
    const handleKeyDown = (e) => {
      if (e.key === 'Escape' && isOpen) {
        onClose();
      }
    };
    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [isOpen, onClose]);

  if (!isOpen) return null;

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div 
        className="modal-content" 
        style={{ maxWidth }} 
        onClick={(e) => e.stopPropagation()}
      >
        <div className="modal-header">
          <h3 style={{ fontSize: '17px', fontWeight: 700, color: '#1E293B', letterSpacing: '-0.015em' }}>
            {title}
          </h3>
          <button 
            type="button" 
            onClick={onClose}
            style={{ 
              background: '#F5F9FF', 
              border: '1px solid #E2EAF2', 
              cursor: 'pointer', 
              color: '#64748B',
              padding: '5px',
              borderRadius: '8px',
              display: 'flex',
              alignItems: 'center',
              transition: 'all 0.15s ease'
            }}
          >
            <X size={18} />
          </button>
        </div>
        <div style={{ padding: '24px 26px' }}>
          {children}
        </div>
      </div>
    </div>
  );
}
