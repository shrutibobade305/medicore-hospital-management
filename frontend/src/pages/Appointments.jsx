import React, { useState, useEffect } from 'react';
import { 
  Calendar, 
  Clock, 
  Plus, 
  Search, 
  CheckCircle2, 
  XCircle, 
  AlertCircle, 
  User, 
  Layers, 
  Check, 
  X,
  Filter,
  ArrowRight,
  UserCheck,
  Zap,
  ListOrdered
} from 'lucide-react';
import { api } from '../services/api';
import Modal from '../components/Modal';
import { useToast } from '../components/Toast';
import { getDoctorImage } from '../utils/doctorImages';

export default function Appointments() {
  const [appointments, setAppointments] = useState([]);
  const [waitingQueue, setWaitingQueue] = useState([]);
  const [patients, setPatients] = useState([]);
  const [doctors, setDoctors] = useState([]);
  const [loading, setLoading] = useState(true);

  // Filters
  const [filterDate, setFilterDate] = useState('');
  const [filterDoctor, setFilterDoctor] = useState('');
  const [filterStatus, setFilterStatus] = useState('');

  // Modals
  const [isBookModalOpen, setIsBookModalOpen] = useState(false);
  const [editingAppt, setEditingAppt] = useState(null);

  const { addToast } = useToast();

  const [form, setForm] = useState({
    patientId: '',
    doctorId: '',
    appointmentDate: new Date().toISOString().substring(0, 10),
    appointmentTime: '10:00',
    reason: '',
    status: 'Scheduled'
  });

  const loadData = async () => {
    try {
      setLoading(true);
      const apptRes = await api.getAppointments({
        date: filterDate,
        doctorId: filterDoctor,
        status: filterStatus
      });
      setAppointments(apptRes.data || []);

      const qRes = await api.getWaitingQueue();
      setWaitingQueue(qRes.data || []);

      const pts = await api.getPatients();
      setPatients(pts.data || []);

      const dcs = await api.getDoctors();
      setDoctors(dcs.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load appointments', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadData();
  }, [filterDate, filterDoctor, filterStatus]);

  const handleOpenBook = () => {
    setForm({
      patientId: patients[0]?.id || '',
      doctorId: doctors[0]?.id || '',
      appointmentDate: new Date().toISOString().substring(0, 10),
      appointmentTime: '10:00',
      reason: '',
      status: 'Scheduled'
    });
    setIsBookModalOpen(true);
  };

  const handleOpenEdit = (a) => {
    setEditingAppt(a);
    setForm(a);
  };

  const handleSaveAppointment = async (e) => {
    e.preventDefault();
    try {
      if (editingAppt) {
        await api.updateAppointment(editingAppt.id, form);
        addToast(`Appointment ${editingAppt.id} updated!`, 'success');
        setEditingAppt(null);
      } else {
        const res = await api.createAppointment(form);
        addToast(`Appointment ${res.data.id} scheduled & queued!`, 'success');
        setIsBookModalOpen(false);
      }
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleUpdateStatus = async (id, newStatus) => {
    try {
      await api.updateAppointment(id, { status: newStatus });
      addToast(`Appointment status updated to ${newStatus}!`, 'success');
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleNextInQueue = async () => {
    if (waitingQueue.length === 0) return;
    const nextAppt = waitingQueue[0];
    try {
      await api.updateAppointment(nextAppt.id, { status: 'Completed' });
      addToast(`Consultation for ${nextAppt.patientName} dequeued from FIFO Queue and marked Completed!`, 'success');
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

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
              Appointments & Consultation Flow
            </h2>
            <span className="badge badge-teal">Outpatient Queue & Scheduling</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Manage physician outpatient bookings and consultation waiting room flow.
          </p>
        </div>

        <div style={{ display: 'flex', gap: '10px' }}>
          <button
            type="button"
            className="btn btn-primary"
            onClick={handleNextInQueue}
            disabled={waitingQueue.length === 0}
            title="Call next patient in consultation line"
          >
            <Zap size={16} /> Call Next Patient ({waitingQueue.length})
          </button>
          <button type="button" className="btn btn-secondary" onClick={handleOpenBook}>
            <Plus size={16} /> Book Appointment
          </button>
        </div>
      </div>

      {/* Top Banner: Waiting Room Queue Tracker */}
      <div className="card" style={{ padding: '18px 24px', marginBottom: '22px', borderLeft: '4px solid var(--primary-deep)' }}>
        <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '12px' }}>
          <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
            <div style={{
              width: '40px',
              height: '40px',
              borderRadius: '12px',
              background: 'var(--mint-tint)',
              color: 'var(--primary-deep)',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center'
            }}>
              <ListOrdered size={22} />
            </div>
            <div>
              <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Active Consultation Waiting Queue
              </div>
              <div style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                {waitingQueue.length === 0
                  ? 'No patients currently waiting in the consultation lounge.'
                  : `${waitingQueue.length} patient${waitingQueue.length > 1 ? 's' : ''} currently queued in arrival order.`}
              </div>
            </div>
          </div>

          {waitingQueue.length > 0 && (
            <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
              {waitingQueue.slice(0, 4).map((q, idx) => (
                <div
                  key={q.id}
                  style={{
                    padding: '6px 12px',
                    background: idx === 0 ? '#E6F3F0' : '#FAF8F5',
                    border: idx === 0 ? '1px solid #B0DAD8' : '1px solid #E5E7E4',
                    borderRadius: '8px',
                    fontSize: '12px',
                    display: 'flex',
                    alignItems: 'center',
                    gap: '6px'
                  }}
                >
                  <span style={{
                    width: '18px',
                    height: '18px',
                    borderRadius: '50%',
                    background: idx === 0 ? '#4F9F9F' : '#9DA7AA',
                    color: '#FFFFFF',
                    fontSize: '10px',
                    fontWeight: 700,
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'center'
                  }}>
                    {idx + 1}
                  </span>
                  <span style={{ fontWeight: 600, color: '#27343A' }}>{q.patientName}</span>
                  <span style={{ color: '#707B7F' }}>({q.appointmentTime})</span>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>

      {/* Filter Bar */}
      <div className="card" style={{ padding: '16px 22px', marginBottom: '22px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px', flexWrap: 'wrap' }}>
          
          <div style={{ minWidth: '170px' }}>
            <label style={{ fontSize: '11px', fontWeight: 700, textTransform: 'uppercase', color: '#707B7F', display: 'block', marginBottom: '4px' }}>
              Filter by Date
            </label>
            <input
              type="date"
              className="form-input"
              value={filterDate}
              onChange={(e) => setFilterDate(e.target.value)}
            />
          </div>

          <div style={{ minWidth: '200px' }}>
            <label style={{ fontSize: '11px', fontWeight: 700, textTransform: 'uppercase', color: '#707B7F', display: 'block', marginBottom: '4px' }}>
              Filter by Doctor
            </label>
            <select
              className="form-select"
              value={filterDoctor}
              onChange={(e) => setFilterDoctor(e.target.value)}
            >
              <option value="">All Physicians</option>
              {doctors.map((d) => (
                <option key={d.id} value={d.id}>{d.name} ({d.department})</option>
              ))}
            </select>
          </div>

          <div style={{ minWidth: '160px' }}>
            <label style={{ fontSize: '11px', fontWeight: 700, textTransform: 'uppercase', color: '#707B7F', display: 'block', marginBottom: '4px' }}>
              Status
            </label>
            <select
              className="form-select"
              value={filterStatus}
              onChange={(e) => setFilterStatus(e.target.value)}
            >
              <option value="">All Statuses</option>
              <option value="Scheduled">Scheduled</option>
              <option value="Waiting">Waiting (In Queue)</option>
              <option value="Completed">Completed</option>
              <option value="Cancelled">Cancelled</option>
            </select>
          </div>

          {(filterDate || filterDoctor || filterStatus) && (
            <div style={{ alignSelf: 'flex-end' }}>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => { setFilterDate(''); setFilterDoctor(''); setFilterStatus(''); }}
              >
                Reset Filters
              </button>
            </div>
          )}

          <div style={{ marginLeft: 'auto', alignSelf: 'flex-end', fontSize: '13px', color: '#707B7F', fontWeight: 600 }}>
            {appointments.length} Consultations Listed
          </div>
        </div>
      </div>

      {/* Appointments Table */}
      <div className="card">
        <div className="table-container" style={{ border: 'none' }}>
          <table className="table">
            <thead>
              <tr>
                <th>Appointment ID</th>
                <th>Patient Details</th>
                <th>Assigned Doctor</th>
                <th>Schedule Slot</th>
                <th>Chief Complaint</th>
                <th>Status</th>
                <th style={{ textAlign: 'right' }}>Actions</th>
              </tr>
            </thead>
            <tbody>
              {loading ? (
                <tr>
                  <td colSpan="7" style={{ textAlign: 'center', padding: '40px 0', color: '#707B7F' }}>
                    Loading appointment schedule...
                  </td>
                </tr>
              ) : appointments.length === 0 ? (
                <tr>
                  <td colSpan="7" style={{ textAlign: 'center', padding: '40px 0', color: '#707B7F' }}>
                    No consultation records found matching filter criteria.
                  </td>
                </tr>
              ) : (
                appointments.map((a) => (
                  <tr key={a.id}>
                    <td>
                      <span style={{
                        fontFamily: 'monospace',
                        fontWeight: 700,
                        color: '#326B6B',
                        background: '#E6F3F0',
                        padding: '3px 8px',
                        borderRadius: '6px',
                        border: '1px solid #B0DAD8',
                        fontSize: '12.5px'
                      }}>
                        {a.id}
                      </span>
                    </td>
                    <td>
                      <div style={{ fontWeight: 700, color: '#27343A' }}>{a.patientName}</div>
                      <div style={{ fontSize: '11.5px', color: '#9DA7AA' }}>{a.patientId}</div>
                    </td>
                    <td>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
                        <div style={{
                          width: '32px',
                          height: '32px',
                          borderRadius: '50%',
                          padding: '2px',
                          background: 'linear-gradient(135deg, var(--primary-light), var(--primary-deep))',
                          flexShrink: 0
                        }}>
                          <img
                            src={getDoctorImage(a.doctorId, a.doctorName)}
                            alt={a.doctorName}
                            style={{ width: '100%', height: '100%', borderRadius: '50%', objectFit: 'cover' }}
                            onError={(e) => { e.target.src = '/doctors/doc-1.png'; }}
                          />
                        </div>
                        <div>
                          <div style={{ fontWeight: 600, color: 'var(--text-primary)' }}>{a.doctorName}</div>
                          <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)' }}>{a.doctorId}</div>
                        </div>
                      </div>
                    </td>
                    <td>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '5px', fontSize: '13px', color: '#27343A', fontWeight: 500 }}>
                        <Calendar size={13} style={{ color: '#4F9F9F' }} />
                        <span>{a.appointmentDate}</span>
                      </div>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '5px', fontSize: '12px', color: '#707B7F', marginTop: '2px' }}>
                        <Clock size={12} style={{ color: '#9DA7AA' }} />
                        <span>{a.appointmentTime}</span>
                      </div>
                    </td>
                    <td>
                      <span style={{ fontSize: '13px', color: '#3A474D' }}>
                        {a.reason || 'General Follow-up'}
                      </span>
                    </td>
                    <td>
                      <span className={`badge ${
                        a.status === 'Waiting' ? 'badge-high' :
                        a.status === 'Completed' ? 'badge-available' :
                        a.status === 'Scheduled' ? 'badge-info' : 'badge-maintenance'
                      }`}>
                        {a.status}
                      </span>
                    </td>
                    <td style={{ textAlign: 'right' }}>
                      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'flex-end', gap: '6px' }}>
                        {a.status === 'Scheduled' && (
                          <button
                            type="button"
                            className="btn btn-secondary btn-sm"
                            style={{ padding: '5px 9px', color: '#B88528' }}
                            onClick={() => handleUpdateStatus(a.id, 'Waiting')}
                            title="Patient arrived — Put in FIFO Waiting Queue"
                          >
                            Mark Waiting
                          </button>
                        )}
                        {a.status === 'Waiting' && (
                          <button
                            type="button"
                            className="btn btn-teal btn-sm"
                            style={{ padding: '5px 9px' }}
                            onClick={() => handleUpdateStatus(a.id, 'Completed')}
                            title="Complete consultation"
                          >
                            Complete
                          </button>
                        )}
                        <button
                          type="button"
                          className="btn btn-secondary btn-sm"
                          style={{ padding: '5px 9px' }}
                          onClick={() => handleOpenEdit(a)}
                          title="Edit Appointment"
                        >
                          Edit
                        </button>
                      </div>
                    </td>
                  </tr>
                ))
              )}
            </tbody>
          </table>
        </div>
      </div>

      {/* MODAL: Book / Edit Appointment */}
      <Modal
        isOpen={isBookModalOpen || !!editingAppt}
        onClose={() => { setIsBookModalOpen(false); setEditingAppt(null); }}
        title={editingAppt ? `Modify Consultation (${editingAppt.id})` : 'Schedule Clinical Consultation'}
      >
        <form onSubmit={handleSaveAppointment}>
          <div className="form-group">
            <label className="form-label">Select Patient *</label>
            <select
              required
              className="form-select"
              value={form.patientId}
              onChange={(e) => setForm({ ...form, patientId: e.target.value })}
            >
              <option value="">-- Choose Registered Patient --</option>
              {patients.map((p) => (
                <option key={p.id} value={p.id}>{p.name} ({p.id})</option>
              ))}
            </select>
          </div>

          <div className="form-group">
            <label className="form-label">Select Doctor *</label>
            <select
              required
              className="form-select"
              value={form.doctorId}
              onChange={(e) => setForm({ ...form, doctorId: e.target.value })}
            >
              <option value="">-- Choose Specialist Doctor --</option>
              {doctors.map((d) => (
                <option key={d.id} value={d.id}>{d.name} ({d.department})</option>
              ))}
            </select>
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '16px' }}>
            <div className="form-group">
              <label className="form-label">Consultation Date *</label>
              <input
                type="date"
                required
                className="form-input"
                value={form.appointmentDate}
                onChange={(e) => setForm({ ...form, appointmentDate: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Time Slot *</label>
              <select
                className="form-select"
                value={form.appointmentTime}
                onChange={(e) => setForm({ ...form, appointmentTime: e.target.value })}
              >
                <option value="09:00">09:00 AM</option>
                <option value="09:30">09:30 AM</option>
                <option value="10:00">10:00 AM</option>
                <option value="10:30">10:30 AM</option>
                <option value="11:00">11:00 AM</option>
                <option value="11:30">11:30 AM</option>
                <option value="12:00">12:00 PM</option>
                <option value="14:00">02:00 PM</option>
                <option value="14:30">02:30 PM</option>
                <option value="15:00">03:00 PM</option>
                <option value="15:30">03:30 PM</option>
                <option value="16:00">04:00 PM</option>
              </select>
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Status</label>
            <select
              className="form-select"
              value={form.status}
              onChange={(e) => setForm({ ...form, status: e.target.value })}
            >
              <option value="Scheduled">Scheduled</option>
              <option value="Waiting">Waiting (In Queue)</option>
              <option value="Completed">Completed</option>
              <option value="Cancelled">Cancelled</option>
            </select>
          </div>

          <div className="form-group">
            <label className="form-label">Chief Complaint / Purpose</label>
            <input
              type="text"
              className="form-input"
              placeholder="e.g. Hypertension review, persistent cough"
              value={form.reason}
              onChange={(e) => setForm({ ...form, reason: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '22px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={() => { setIsBookModalOpen(false); setEditingAppt(null); }}
            >
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              {editingAppt ? 'Save Changes' : 'Confirm Appointment'}
            </button>
          </div>
        </form>
      </Modal>
    </div>
  );
}
