import React, { useState, useEffect } from 'react';
import { 
  History, 
  Activity, 
  Download, 
  Filter, 
  Clock, 
  RotateCcw,
  CheckCircle2,
  Layers,
  ShieldCheck,
  FileSpreadsheet
} from 'lucide-react';
import { api } from '../services/api';
import { useToast } from '../components/Toast';

export default function ReportsActivity() {
  const [logs, setLogs] = useState([]);
  const [loading, setLoading] = useState(true);
  const [filterAction, setFilterAction] = useState('');
  const { addToast } = useToast();

  const loadLogs = async () => {
    try {
      setLoading(true);
      const res = await api.getActivity();
      setLogs(res.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load activity logs', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadLogs();
  }, []);

  const filteredLogs = filterAction
    ? logs.filter((l) => l.actionType === filterAction)
    : logs;

  const handleExportJson = () => {
    const dataStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(logs, null, 2));
    const downloadAnchor = document.createElement('a');
    downloadAnchor.setAttribute('href', dataStr);
    downloadAnchor.setAttribute('download', `medicore-activity-log-${new Date().toISOString().substring(0, 10)}.json`);
    document.body.appendChild(downloadAnchor);
    downloadAnchor.click();
    downloadAnchor.remove();
    addToast('Audit log exported to JSON!', 'success');
  };

  const actionTypes = Array.from(new Set(logs.map((l) => l.actionType)));

  return (
    <div className="page-body">
      {/* Header */}
      <div style={{
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        flexWrap: 'wrap',
        gap: '16px',
        marginBottom: '24px'
      }}>
        <div>
          <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
            <h2 style={{ fontSize: '24px', fontWeight: 800, color: 'var(--text-primary)', letterSpacing: '-0.02em' }}>
              System Reports & Clinical Audit Trail
            </h2>
            <span className="badge badge-teal">Audit Trail & Compliance</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Chronological audit log tracking patient registrations, bed allocations, emergency admissions, and clinical events.
          </p>
        </div>

        <button type="button" className="btn btn-secondary" onClick={handleExportJson}>
          <Download size={15} style={{ color: 'var(--primary-deep)' }} /> Export Audit Log (.json)
        </button>
      </div>

      {/* Filter Card */}
      <div className="card" style={{ padding: '16px 22px', marginBottom: '22px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px', flexWrap: 'wrap' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
            <Filter size={16} style={{ color: 'var(--primary-deep)' }} />
            <span style={{ fontSize: '13.5px', fontWeight: 700, color: 'var(--text-primary)' }}>Filter by Event Type:</span>
          </div>

          <select
            className="form-select"
            style={{ width: '260px' }}
            value={filterAction}
            onChange={(e) => setFilterAction(e.target.value)}
          >
            <option value="">All Action Types ({logs.length})</option>
            {actionTypes.map((type) => (
              <option key={type} value={type}>{type}</option>
            ))}
          </select>

          {filterAction && (
            <button type="button" className="btn btn-secondary btn-sm" onClick={() => setFilterAction('')}>
              Reset Filter
            </button>
          )}

          <div style={{ marginLeft: 'auto', fontSize: '13px', color: 'var(--text-secondary)', fontWeight: 600 }}>
            {filteredLogs.length} Events Logged
          </div>
        </div>
      </div>

      {/* Activity Table */}
      <div className="card">
        <div className="table-container" style={{ border: 'none' }}>
          <table className="table">
            <thead>
              <tr>
                <th>Log ID</th>
                <th>Timestamp</th>
                <th>Action Type</th>
                <th>Target Entity</th>
                <th>Clinical Event Description</th>
                <th>Event Status</th>
              </tr>
            </thead>
            <tbody>
              {loading ? (
                <tr>
                  <td colSpan="6" style={{ textAlign: 'center', padding: '40px 0', color: 'var(--text-secondary)' }}>
                    Reading audit logs...
                  </td>
                </tr>
              ) : filteredLogs.length === 0 ? (
                <tr>
                  <td colSpan="6" style={{ textAlign: 'center', padding: '40px 0', color: 'var(--text-secondary)' }}>
                    No audit records found matching criteria.
                  </td>
                </tr>
              ) : (
                filteredLogs.map((log) => {
                  let badgeClass = 'badge-teal';

                  if (log.actionType.includes('EMERGENCY')) {
                    badgeClass = 'badge-danger';
                  } else if (log.actionType.includes('BED')) {
                    badgeClass = 'badge-success';
                  } else if (log.actionType.includes('UNDO')) {
                    badgeClass = 'badge-purple';
                  }

                  return (
                    <tr key={log.id}>
                      <td>
                        <span style={{ fontFamily: 'monospace', fontWeight: 700, color: 'var(--text-secondary)', fontSize: '12px' }}>
                          {log.id}
                        </span>
                      </td>
                      <td>
                        <span style={{ display: 'inline-flex', alignItems: 'center', gap: '5px', fontSize: '12.5px', color: 'var(--text-primary)' }}>
                          <Clock size={12} style={{ color: 'var(--primary-deep)' }} />
                          {log.timestamp}
                        </span>
                      </td>
                      <td>
                        <span className={`badge ${badgeClass}`}>
                          {log.actionType}
                        </span>
                      </td>
                      <td>
                        <strong style={{ color: 'var(--text-primary)' }}>{log.entityName}</strong>: <span style={{ fontFamily: 'monospace', color: 'var(--text-secondary)' }}>{log.entityId}</span>
                      </td>
                      <td style={{ color: 'var(--text-primary)' }}>
                        {log.description}
                      </td>
                      <td>
                        {log.undoable ? (
                          <span className="badge badge-purple" style={{ fontSize: '11px' }}>
                            <RotateCcw size={10} /> Reversible
                          </span>
                        ) : (
                          <span style={{ color: 'var(--text-muted)', fontSize: '12px' }}>Permanent Record</span>
                        )}
                      </td>
                    </tr>
                  );
                })
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}
