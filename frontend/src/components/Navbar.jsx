import React, { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import { RefreshCw, Clock, ChevronRight, Activity, RotateCcw } from 'lucide-react';
import { api } from '../services/api';
import { useToast } from './Toast';

export default function Navbar({ onResetData }) {
  const location = useLocation();
  const [isOnline, setIsOnline] = useState(true);
  const [time, setTime] = useState(new Date().toLocaleTimeString());
  const [date, setDate] = useState(new Date().toLocaleDateString('en-US', { weekday: 'short', month: 'short', day: 'numeric', year: 'numeric' }));
  const [resetting, setResetting] = useState(false);
  const { addToast } = useToast();

  const routeTitles = {
    '/': { title: 'Clinical Dashboard', category: 'Overview' },
    '/patients': { title: 'Patient Registry', category: 'Patients' },
    '/doctors': { title: 'Medical Staff Directory', category: 'Staff' },
    '/appointments': { title: 'Appointments & Consultations', category: 'Outpatient' },
    '/emergency': { title: 'Emergency & Triage Queue', category: 'Critical Care' },
    '/beds': { title: 'Bed & Ward Management', category: 'Facilities' },
    '/navigation': { title: 'Hospital Wayfinding & Floorplan', category: 'Campus' },
    '/reports': { title: 'Activity & Audit Log', category: 'Records' },
    '/settings': { title: 'Hospital Settings', category: 'Administration' },
  };

  const currentInfo = routeTitles[location.pathname] || { title: 'Hospital Management', category: 'System' };

  useEffect(() => {
    const timer = setInterval(() => {
      setTime(new Date().toLocaleTimeString());
    }, 1000);
    return () => clearInterval(timer);
  }, []);

  useEffect(() => {
    const checkHealth = async () => {
      try {
        await api.getHealth();
        setIsOnline(true);
      } catch {
        setIsOnline(false);
      }
    };
    checkHealth();
    const interval = setInterval(checkHealth, 12000);
    return () => clearInterval(interval);
  }, []);

  const handleReset = async () => {
    if (!window.confirm('Reset and reseed demo data? All test records will revert to original demo state.')) return;
    setResetting(true);
    try {
      await api.resetDatabase();
      addToast('Database successfully reset and reseeded with clinical demo records!', 'success');
      if (onResetData) onResetData();
      window.location.reload();
    } catch (err) {
      addToast(err.message || 'Failed to reset database', 'error');
    } finally {
      setResetting(false);
    }
  };

  return (
    <header style={{
      height: '74px',
      background: '#FFFFFF',
      borderBottom: '1px solid var(--border-soft)',
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'space-between',
      padding: '0 36px',
      position: 'sticky',
      top: 0,
      zIndex: 90,
      boxShadow: '0 1px 3px rgba(39, 52, 58, 0.02)'
    }}>
      {/* Left: Dynamic Page Title & Breadcrumb */}
      <div style={{ display: 'flex', flexDirection: 'column' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '6px', fontSize: '12px', color: 'var(--text-secondary)', fontWeight: 500, marginBottom: '2px' }}>
          <span>MediCore</span>
          <ChevronRight size={13} style={{ color: 'var(--text-muted)' }} />
          <span>{currentInfo.category}</span>
          <ChevronRight size={13} style={{ color: 'var(--text-muted)' }} />
          <span style={{ color: 'var(--primary-deep)', fontWeight: 600 }}>{currentInfo.title}</span>
        </div>
        <h1 style={{ fontSize: '19px', fontWeight: 800, color: 'var(--text-primary)', letterSpacing: '-0.015em' }}>
          {currentInfo.title}
        </h1>
      </div>

      {/* Right: Date/Time, Status Badge & Reset Action */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '18px' }}>
        {/* Connection Status Badge */}
        <span style={{
          display: 'flex',
          alignItems: 'center',
          gap: '7px',
          background: isOnline ? 'var(--mint-tint)' : '#FBEBEB',
          color: isOnline ? 'var(--status-success)' : 'var(--status-danger)',
          border: isOnline ? '1px solid var(--primary-light)' : '1px solid #F5C6C6',
          padding: '5px 12px',
          borderRadius: '9999px',
          fontSize: '12px',
          fontWeight: 600
        }}>
          <span style={{
            width: '7px',
            height: '7px',
            borderRadius: '50%',
            background: isOnline ? 'var(--status-success)' : 'var(--status-danger)',
            boxShadow: isOnline ? '0 0 6px var(--status-success)' : 'none'
          }} />
          {isOnline ? 'Hospital Network Online' : 'Network Offline'}
        </span>

        {/* Date & Time */}
        <div style={{ display: 'flex', alignItems: 'center', gap: '8px', color: 'var(--text-secondary)', fontSize: '13px', fontWeight: 500 }}>
          <Clock size={15} style={{ color: 'var(--primary-deep)' }} />
          <span>{date} · {time}</span>
        </div>

        {/* Reset Demo Data Button */}
        <button
          type="button"
          onClick={handleReset}
          disabled={resetting}
          className="btn btn-secondary btn-sm"
          style={{ display: 'flex', alignItems: 'center', gap: '6px' }}
          title="Reset database to fresh clinical demo state"
        >
          <RotateCcw size={14} className={resetting ? 'animate-spin' : ''} style={{ color: 'var(--primary-deep)' }} />
          {resetting ? 'Resetting...' : 'Reset Demo Data'}
        </button>
      </div>
    </header>
  );
}
