import React, { useState, useEffect } from 'react';
import { 
  AlertOctagon, 
  Plus, 
  Zap, 
  Clock, 
  CheckCircle2, 
  XCircle, 
  Flame, 
  AlertTriangle, 
  UserPlus, 
  ArrowRight,
  Filter,
  Check,
  RotateCcw,
  Activity,
  History,
  ShieldAlert,
  HeartPulse
} from 'lucide-react';
import { api } from '../services/api';
import Modal from '../components/Modal';
import { useToast } from '../components/Toast';

export default function EmergencyTriage() {
  const [pendingCases, setPendingCases] = useState([]);
  const [allCases, setAllCases] = useState([]);
  const [loading, setLoading] = useState(true);
  const [isRegisterModalOpen, setIsRegisterModalOpen] = useState(false);
  const [activeTab, setActiveTab] = useState('pending'); // 'pending' or 'history'
  const { addToast } = useToast();

  const [formData, setFormData] = useState({
    patientName: '',
    age: '',
    gender: 'Male',
    condition: '',
    severity: 'Critical',
    notes: ''
  });

  const loadEmergencies = async () => {
    try {
      setLoading(true);
      const [pendRes, allRes] = await Promise.all([
        api.getEmergencyQueue(),
        api.getAllEmergencies()
      ]);
      setPendingCases(pendRes.data || []);
      setAllCases(allRes.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load emergency queue', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadEmergencies();
  }, []);

  const handleRegisterEmergency = async (e) => {
    e.preventDefault();
    try {
      const res = await api.createEmergency({
        ...formData,
        age: parseInt(formData.age || '0', 10)
      });
      addToast(`Emergency intake recorded for ${res.data.patientName}!`, 'success');
      setIsRegisterModalOpen(false);
      setFormData({
        patientName: '',
        age: '',
        gender: 'Male',
        condition: '',
        severity: 'Critical',
        notes: ''
      });
      loadEmergencies();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleDispatchHighest = async () => {
    try {
      const res = await api.dispatchEmergency();
      addToast(res.message || 'Dispatched top priority emergency patient!', 'success');
      loadEmergencies();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleCompleteCase = async (id) => {
    try {
      await api.completeEmergency(id);
      addToast(`Emergency case ${id} marked as treated and resolved!`, 'success');
      loadEmergencies();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const criticalCount = pendingCases.filter((c) => c.severity === 'Critical').length;
  const highCount = pendingCases.filter((c) => c.severity === 'High').length;

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
              Emergency & Trauma Triage Unit
            </h2>
            <span className="badge badge-danger">Emergency & Critical Care</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Immediate clinical prioritization based on triage acuity (Critical &gt; High &gt; Medium &gt; Low).
          </p>
        </div>

        <div style={{ display: 'flex', gap: '10px' }}>
          <button
            type="button"
            className="btn btn-danger"
            onClick={handleDispatchHighest}
            disabled={pendingCases.length === 0}
            title="Dispatch top priority case for immediate care"
          >
            <Zap size={16} /> Dispatch Top Priority Case
          </button>
          <button type="button" className="btn btn-primary" onClick={() => setIsRegisterModalOpen(true)}>
            <Plus size={16} /> Admit Emergency Patient
          </button>
        </div>
      </div>

      {/* KPI Triage Ticker */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
        gap: '16px',
        marginBottom: '24px'
      }}>
        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid var(--status-danger)' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: 'var(--status-danger)', textTransform: 'uppercase' }}>
            Critical (Immediate)
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: 'var(--text-primary)', marginTop: '2px' }}>
            {criticalCount}
          </div>
          <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)' }}>Life-threatening condition</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid var(--status-warning)' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: 'var(--status-warning)', textTransform: 'uppercase' }}>
            High Priority
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: 'var(--text-primary)', marginTop: '2px' }}>
            {highCount}
          </div>
          <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)' }}>Severe emergency (&lt;15m)</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid var(--primary-deep)' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: 'var(--primary-deep)', textTransform: 'uppercase' }}>
            Total Pending Intake
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: 'var(--text-primary)', marginTop: '2px' }}>
            {pendingCases.length}
          </div>
          <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)' }}>Awaiting clinical dispatch</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid var(--status-success)' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: 'var(--status-success)', textTransform: 'uppercase' }}>
            Total Resolved
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: 'var(--text-primary)', marginTop: '2px' }}>
            {allCases.filter((c) => c.status === 'Completed').length}
          </div>
          <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)' }}>Stabilized & treated</div>
        </div>
      </div>

      {/* Tabs */}
      <div style={{ display: 'flex', borderBottom: '1px solid var(--border-soft)', marginBottom: '22px' }}>
        <button
          type="button"
          onClick={() => setActiveTab('pending')}
          style={{
            padding: '12px 22px',
            fontSize: '13.5px',
            fontWeight: 700,
            borderBottom: activeTab === 'pending' ? '2px solid var(--primary-deep)' : '2px solid transparent',
            color: activeTab === 'pending' ? 'var(--primary-deep)' : 'var(--text-secondary)',
            background: activeTab === 'pending' ? 'var(--mint-tint)' : 'transparent',
            borderTopLeftRadius: '8px',
            borderTopRightRadius: '8px',
            cursor: 'pointer',
            borderTop: 'none',
            borderLeft: 'none',
            borderRight: 'none',
            display: 'flex',
            alignItems: 'center',
            gap: '8px',
            transition: 'all 0.15s ease'
          }}
        >
          <Flame size={16} /> Live Triage Queue ({pendingCases.length})
        </button>

        <button
          type="button"
          onClick={() => setActiveTab('history')}
          style={{
            padding: '12px 22px',
            fontSize: '13.5px',
            fontWeight: 700,
            borderBottom: activeTab === 'history' ? '2px solid var(--primary-deep)' : '2px solid transparent',
            color: activeTab === 'history' ? 'var(--primary-deep)' : 'var(--text-secondary)',
            background: activeTab === 'history' ? 'var(--mint-tint)' : 'transparent',
            borderTopLeftRadius: '8px',
            borderTopRightRadius: '8px',
            cursor: 'pointer',
            borderTop: 'none',
            borderLeft: 'none',
            borderRight: 'none',
            display: 'flex',
            alignItems: 'center',
            gap: '8px',
            transition: 'all 0.15s ease'
          }}
        >
          <History size={16} /> Triage History Log ({allCases.length})
        </button>
      </div>

      {/* TAB 1: Live Triage Queue */}
      {activeTab === 'pending' && (
        <div className="card">
          <div className="card-header">
            <div>
              <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Active Emergency Triage Queue
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Patients prioritized by severity and arrival time for immediate resuscitation.
              </p>
            </div>
            {pendingCases.length > 0 && (
              <span className="badge badge-danger">
                Next: {pendingCases[0].patientName} ({pendingCases[0].severity})
              </span>
            )}
          </div>

          <div className="table-container" style={{ border: 'none' }}>
            <table className="table">
              <thead>
                <tr>
                  <th style={{ width: '60px' }}>Rank</th>
                  <th>Case ID</th>
                  <th>Patient Info</th>
                  <th>Presenting Condition / Trauma</th>
                  <th>Triage Acuity</th>
                  <th>Arrival Time</th>
                  <th>Status</th>
                  <th style={{ textAlign: 'right' }}>Actions</th>
                </tr>
              </thead>
              <tbody>
                {loading ? (
                  <tr>
                    <td colSpan="8" style={{ textAlign: 'center', padding: '40px 0', color: 'var(--text-secondary)' }}>
                      Loading emergency queue...
                    </td>
                  </tr>
                ) : pendingCases.length === 0 ? (
                  <tr>
                    <td colSpan="8" style={{ textAlign: 'center', padding: '50px 0', color: 'var(--text-secondary)' }}>
                      <CheckCircle2 size={40} style={{ color: 'var(--status-success)', margin: '0 auto 10px auto' }} />
                      <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)' }}>Triage Queue is Clear</div>
                      <div style={{ fontSize: '13px' }}>No active trauma or critical emergency cases waiting.</div>
                    </td>
                  </tr>
                ) : (
                  pendingCases.map((item, idx) => (
                    <tr
                      key={item.id}
                      style={{
                        background: idx === 0 ? '#FDF6F6' : 'transparent'
                      }}
                    >
                      <td>
                        <span style={{
                          width: '28px',
                          height: '28px',
                          borderRadius: '50%',
                          background: idx === 0 ? 'var(--status-danger)' : 'var(--mint-tint)',
                          color: idx === 0 ? '#FFFFFF' : 'var(--primary-deep)',
                          fontSize: '12px',
                          fontWeight: 800,
                          display: 'flex',
                          alignItems: 'center',
                          justifyContent: 'center'
                        }}>
                          #{idx + 1}
                        </span>
                      </td>
                      <td>
                        <span style={{ fontFamily: 'monospace', fontWeight: 700, color: 'var(--text-secondary)', fontSize: '12px' }}>
                          {item.id}
                        </span>
                      </td>
                      <td>
                        <div style={{ fontWeight: 700, color: 'var(--text-primary)', fontSize: '14px' }}>
                          {item.patientName}
                        </div>
                        <div style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
                          {item.age}y · {item.gender}
                        </div>
                      </td>
                      <td>
                        <div style={{ fontWeight: 600, color: 'var(--text-primary)' }}>{item.condition}</div>
                        {item.notes && (
                          <div style={{ fontSize: '12px', color: 'var(--text-secondary)', maxWidth: '240px', overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                            {item.notes}
                          </div>
                        )}
                      </td>
                      <td>
                        <span className={`badge ${
                          item.severity === 'Critical' ? 'badge-danger' :
                          item.severity === 'High' ? 'badge-warning' :
                          item.severity === 'Medium' ? 'badge-teal' : 'badge-available'
                        }`}>
                          {item.severity}
                        </span>
                      </td>
                      <td>
                        <div style={{ display: 'flex', alignItems: 'center', gap: '4px', fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                          <Clock size={12} style={{ color: 'var(--primary-deep)' }} />
                          {item.arrivalTime}
                        </div>
                      </td>
                      <td>
                        <span className="badge badge-warning">
                          {item.status}
                        </span>
                      </td>
                      <td style={{ textAlign: 'right' }}>
                        {idx === 0 ? (
                          <button
                            type="button"
                            className="btn btn-danger btn-sm"
                            onClick={handleDispatchHighest}
                          >
                            <Zap size={13} /> Dispatch Now
                          </button>
                        ) : (
                          <button
                            type="button"
                            className="btn btn-secondary btn-sm"
                            onClick={() => handleCompleteCase(item.id)}
                          >
                            <Check size={13} /> Resolve
                          </button>
                        )}
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* TAB 2: Historical Logs */}
      {activeTab === 'history' && (
        <div className="card">
          <div className="card-header">
            <div>
              <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Emergency Department Treatment Logs
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Permanent audit log of all triaged emergency admissions
              </p>
            </div>
          </div>

          <div className="table-container" style={{ border: 'none' }}>
            <table className="table">
              <thead>
                <tr>
                  <th>Case ID</th>
                  <th>Patient Name</th>
                  <th>Diagnosis / Trauma</th>
                  <th>Triage Acuity</th>
                  <th>Arrival Time</th>
                  <th>Resolution Status</th>
                </tr>
              </thead>
              <tbody>
                {allCases.map((c) => (
                  <tr key={c.id}>
                    <td>
                      <span style={{ fontFamily: 'monospace', fontWeight: 700, color: 'var(--text-secondary)', fontSize: '12px' }}>
                        {c.id}
                      </span>
                    </td>
                    <td>
                      <strong style={{ color: 'var(--text-primary)' }}>{c.patientName}</strong> ({c.age}y, {c.gender})
                    </td>
                    <td style={{ color: 'var(--text-primary)' }}>{c.condition}</td>
                    <td>
                      <span className={`badge ${c.severity === 'Critical' ? 'badge-danger' : c.severity === 'High' ? 'badge-warning' : 'badge-teal'}`}>
                        {c.severity}
                      </span>
                    </td>
                    <td style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>{c.arrivalTime}</td>
                    <td>
                      <span className={`badge ${c.status === 'Completed' ? 'badge-available' : c.status === 'Dispatched' ? 'badge-info' : 'badge-warning'}`}>
                        {c.status}
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* MODAL: Register New Emergency */}
      <Modal
        isOpen={isRegisterModalOpen}
        onClose={() => setIsRegisterModalOpen(false)}
        title="Emergency Patient Intake & Triage"
      >
        <form onSubmit={handleRegisterEmergency}>
          <div className="form-group">
            <label className="form-label">Patient Name *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="Patient Name or Unknown/Trauma"
              value={formData.patientName}
              onChange={(e) => setFormData({ ...formData, patientName: e.target.value })}
            />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr 1fr', gap: '12px' }}>
            <div className="form-group">
              <label className="form-label">Age</label>
              <input
                type="number"
                min="0"
                max="125"
                className="form-input"
                placeholder="Age"
                value={formData.age}
                onChange={(e) => setFormData({ ...formData, age: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Gender</label>
              <select
                className="form-select"
                value={formData.gender}
                onChange={(e) => setFormData({ ...formData, gender: e.target.value })}
              >
                <option value="Male">Male</option>
                <option value="Female">Female</option>
                <option value="Other">Other</option>
              </select>
            </div>
            <div className="form-group">
              <label className="form-label">Triage Severity *</label>
              <select
                required
                className="form-select"
                value={formData.severity}
                onChange={(e) => setFormData({ ...formData, severity: e.target.value })}
              >
                <option value="Critical">Critical (Priority 1)</option>
                <option value="High">High (Priority 2)</option>
                <option value="Medium">Medium (Priority 3)</option>
                <option value="Low">Low (Priority 4)</option>
              </select>
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Presenting Condition / Diagnosis *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Acute Myocardial Infarction, Multiple Trauma"
              value={formData.condition}
              onChange={(e) => setFormData({ ...formData, condition: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Triage & Resuscitation Notes</label>
            <textarea
              className="form-textarea"
              placeholder="Vital signs, GCS, blood pressure, airway assessment..."
              value={formData.notes}
              onChange={(e) => setFormData({ ...formData, notes: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={() => setIsRegisterModalOpen(false)}
            >
              Cancel
            </button>
            <button type="submit" className="btn btn-danger">
              Admit Emergency Case
            </button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
