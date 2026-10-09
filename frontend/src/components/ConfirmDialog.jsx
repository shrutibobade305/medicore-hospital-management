import React from 'react';
import Modal from './Modal';
import { AlertTriangle, AlertCircle } from 'lucide-react';

export default function ConfirmDialog({ isOpen, onClose, onConfirm, title, message, confirmText = 'Confirm Delete', isDanger = true }) {
  return (
    <Modal isOpen={isOpen} onClose={onClose} title={title} maxWidth="480px">
      <div style={{ display: 'flex', gap: '16px', alignItems: 'flex-start' }}>
        <div style={{
          width: '44px',
          height: '44px',
          borderRadius: '12px',
          background: isDanger ? '#FEE2E2' : '#FEF3C7',
          color: isDanger ? '#DC2626' : '#D97706',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          flexShrink: 0,
          border: `1px solid ${isDanger ? '#FECACA' : '#FDE68A'}`
        }}>
          {isDanger ? <AlertTriangle size={22} /> : <AlertCircle size={22} />}
        </div>
        <div style={{ flex: 1 }}>
          <p style={{ fontSize: '13.5px', color: '#475569', lineHeight: 1.6, marginBottom: '22px' }}>
            {message}
          </p>
          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px' }}>
            <button type="button" className="btn btn-secondary btn-sm" onClick={onClose}>
              Cancel
            </button>
            <button 
              type="button" 
              className={`btn ${isDanger ? 'btn-danger' : 'btn-primary'} btn-sm`} 
              onClick={() => {
                onConfirm();
                onClose();
              }}
            >
              {confirmText}
            </button>
          </div>
        </div>
      </div>
    </Modal>
  );
}
