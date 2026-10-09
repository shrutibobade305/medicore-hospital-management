import React from 'react';
import { NavLink } from 'react-router-dom';
import {
  LayoutDashboard,
  Users,
  UserCheck,
  Calendar,
  AlertOctagon,
  BedDouble,
  Navigation,
  History,
  Settings,
  ChevronLeft,
  ChevronRight,
  HeartPulse
} from 'lucide-react';

export default function Sidebar({ isCollapsed, setIsCollapsed, pendingEmergencies = 0 }) {
  const navItems = [
    { to: '/', label: 'Dashboard', icon: LayoutDashboard },
    { to: '/patients', label: 'Patients', icon: Users },
    { to: '/doctors', label: 'Doctors', icon: UserCheck },
    { to: '/appointments', label: 'Appointments', icon: Calendar },
    { 
      to: '/emergency', 
      label: 'Emergency & Triage', 
      icon: AlertOctagon, 
      badge: pendingEmergencies > 0 ? pendingEmergencies : null 
    },
    { to: '/beds', label: 'Bed Management', icon: BedDouble },
    { to: '/navigation', label: 'Hospital Navigation', icon: Navigation },
    { to: '/reports', label: 'Activity & History', icon: History },
    { to: '/settings', label: 'Hospital Settings', icon: Settings },
  ];

  return (
    <aside style={{
      width: isCollapsed ? '78px' : '264px',
      background: '#FFFFFF',
      borderRight: '1px solid var(--border-soft)',
      display: 'flex',
      flexDirection: 'column',
      transition: 'width 0.22s cubic-bezier(0.4, 0, 0.2, 1)',
      position: 'sticky',
      top: 0,
      height: '100vh',
      zIndex: 100,
      flexShrink: 0
    }}>
      {/* Brand Header */}
      <div style={{
        padding: isCollapsed ? '18px 12px' : '18px 20px',
        borderBottom: '1px solid var(--border-soft)',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        height: '74px',
        background: '#FFFFFF'
      }}>
        {!isCollapsed && (
          <div style={{ display: 'flex', alignItems: 'center', gap: '11px' }}>
            <div style={{
              width: '38px',
              height: '38px',
              borderRadius: '12px',
              background: 'linear-gradient(135deg, var(--primary-deep) 0%, var(--primary-teal) 100%)',
              color: '#FFFFFF',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              boxShadow: '0 3px 8px rgba(79, 159, 159, 0.28)'
            }}>
              <HeartPulse size={22} />
            </div>
            <div>
              <div style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)', letterSpacing: '-0.02em', lineHeight: 1.15 }}>
                MediCore
              </div>
              <div style={{ fontSize: '9.5px', fontWeight: 700, color: 'var(--primary-deep)', letterSpacing: '0.06em', textTransform: 'uppercase', marginTop: '2px' }}>
                HOSPITAL MANAGEMENT SYSTEM
              </div>
            </div>
          </div>
        )}

        {isCollapsed && (
          <div style={{
            width: '38px',
            height: '38px',
            borderRadius: '12px',
            background: 'linear-gradient(135deg, var(--primary-deep) 0%, var(--primary-teal) 100%)',
            color: '#FFFFFF',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            boxShadow: '0 3px 8px rgba(79, 159, 159, 0.25)'
          }}>
            <HeartPulse size={22} />
          </div>
        )}

        <button
          type="button"
          onClick={() => setIsCollapsed(!isCollapsed)}
          style={{
            background: 'var(--bg-canvas)',
            border: '1px solid var(--border-soft)',
            borderRadius: '8px',
            width: '28px',
            height: '28px',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            cursor: 'pointer',
            color: 'var(--text-secondary)',
            transition: 'all 0.15s ease'
          }}
          title={isCollapsed ? 'Expand Sidebar' : 'Collapse Sidebar'}
        >
          {isCollapsed ? <ChevronRight size={15} /> : <ChevronLeft size={15} />}
        </button>
      </div>

      {/* Nav List */}
      <nav style={{ flex: 1, padding: '16px 12px', overflowY: 'auto', display: 'flex', flexDirection: 'column', gap: '5px' }}>
        {navItems.map((item) => {
          const Icon = item.icon;
          return (
            <NavLink
              key={item.to}
              to={item.to}
              style={({ isActive }) => ({
                display: 'flex',
                alignItems: 'center',
                gap: '12px',
                padding: isCollapsed ? '11px 0' : '10px 14px',
                justifyContent: isCollapsed ? 'center' : 'flex-start',
                borderRadius: '11px',
                textDecoration: 'none',
                fontWeight: isActive ? 700 : 500,
                fontSize: '13.5px',
                color: isActive ? 'var(--primary-deep)' : 'var(--text-secondary)',
                background: isActive ? 'var(--mint-tint)' : 'transparent',
                border: isActive ? '1px solid var(--primary-light)' : '1px solid transparent',
                transition: 'all 0.15s ease',
                position: 'relative'
              })}
              title={isCollapsed ? item.label : undefined}
            >
              <Icon size={19} style={{ flexShrink: 0 }} />
              
              {!isCollapsed && (
                <span style={{ flex: 1, whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis' }}>
                  {item.label}
                </span>
              )}

              {!isCollapsed && item.badge && (
                <span style={{
                  background: 'var(--status-danger)',
                  color: '#FFFFFF',
                  borderRadius: '9999px',
                  padding: '2px 8px',
                  fontSize: '11px',
                  fontWeight: 700,
                  boxShadow: '0 2px 4px rgba(201, 86, 86, 0.25)'
                }}>
                  {item.badge}
                </span>
              )}

              {isCollapsed && item.badge && (
                <span style={{
                  position: 'absolute',
                  top: '5px',
                  right: '14px',
                  width: '8px',
                  height: '8px',
                  background: 'var(--status-danger)',
                  borderRadius: '50%'
                }} />
              )}
            </NavLink>
          );
        })}
      </nav>

      {/* Hospital Status Footer */}
      {!isCollapsed && (
        <div style={{ padding: '16px 18px', borderTop: '1px solid var(--border-soft)', background: 'var(--bg-card-sub)' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '3px' }}>
            <span style={{ width: '8px', height: '8px', borderRadius: '50%', background: 'var(--status-success)', boxShadow: '0 0 6px var(--status-success)' }}></span>
            <span style={{ fontSize: '11.5px', fontWeight: 700, color: 'var(--text-primary)' }}>MediCore Live Network</span>
          </div>
          <p style={{ fontSize: '11px', color: 'var(--text-secondary)' }}>
            All Systems Operational
          </p>
        </div>
      )}
    </aside>
  );
}
