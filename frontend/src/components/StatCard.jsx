import React from 'react';

export default function StatCard({ title, value, subtext, subtitle, icon: Icon, variant = 'teal', color, tag, trend }) {
  const chosenColor = color || variant;
  const colorMap = {
    teal: { bg: 'var(--mint-tint)', text: 'var(--primary-deep)', border: 'var(--primary-light)' },
    aqua: { bg: 'var(--mint-tint)', text: '#3A8787', border: 'var(--primary-light)' },
    success: { bg: '#EBF5F0', text: 'var(--status-success)', border: '#C4E5D4' },
    emerald: { bg: '#EBF5F0', text: 'var(--status-success)', border: '#C4E5D4' },
    warning: { bg: '#FCF5E8', text: '#B88528', border: '#F6DFB3' },
    amber: { bg: '#FCF5E8', text: '#B88528', border: '#F6DFB3' },
    danger: { bg: '#FBEBEB', text: 'var(--status-danger)', border: '#F5C6C6' },
    rose: { bg: '#FBEBEB', text: 'var(--status-danger)', border: '#F5C6C6' },
    purple: { bg: '#EEF2F8', text: '#556688', border: '#D8E0EE' },
  };

  const scheme = colorMap[chosenColor] || colorMap.teal;
  const displaySubtext = subtext || subtitle;

  return (
    <div className="card" style={{ padding: '22px 24px', position: 'relative', overflow: 'hidden' }}>
      <div style={{ display: 'flex', alignItems: 'flex-start', justifyContent: 'space-between', marginBottom: '14px' }}>
        <div>
          <span style={{ fontSize: '12px', fontWeight: 700, color: 'var(--text-secondary)', textTransform: 'uppercase', letterSpacing: '0.04em' }}>
            {title}
          </span>
          <div style={{ fontSize: '30px', fontWeight: 800, color: 'var(--text-primary)', marginTop: '4px', lineHeight: 1.1, letterSpacing: '-0.02em' }}>
            {value}
          </div>
        </div>
        <div style={{
          width: '46px',
          height: '46px',
          borderRadius: '14px',
          background: scheme.bg,
          color: scheme.text,
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          flexShrink: 0,
          border: `1px solid ${scheme.border}`
        }}>
          {Icon && <Icon size={22} />}
        </div>
      </div>

      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', fontSize: '12.5px', marginTop: '6px' }}>
        {displaySubtext && (
          <span style={{ color: 'var(--text-secondary)', fontWeight: 500 }}>{displaySubtext}</span>
        )}
        {tag && (
          <span className="badge badge-teal">
            {tag}
          </span>
        )}
      </div>

      {trend && (
        <div style={{ marginTop: '8px', fontSize: '12px', color: trend.positive ? 'var(--status-success)' : 'var(--status-danger)', fontWeight: 600 }}>
          {trend.positive ? '↑' : '↓'} {trend.text}
        </div>
      )}
    </div>
  );
}
