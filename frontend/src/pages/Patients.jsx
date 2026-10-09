import React, { useState, useEffect } from 'react';
import { 
  Users, 
  Search, 
  Plus, 
  Edit3, 
  Trash2, 
  Eye, 
  Calendar, 
  Clock, 
  Phone, 
  MapPin, 
  FileText, 
  Activity,
  ArrowUpDown,
  Filter,
  CheckCircle2,
  XCircle,
  Stethoscope,
  Heart
} from 'lucide-react';
import { api } from '../services/api';
import Modal from '../components/Modal';
import ConfirmDialog from '../components/ConfirmDialog';
import { useToast } from '../components/Toast';

export default function Patients() {
  const [patients, setPatients] = useState([]);
  const [loading, setLoading] = useState(true);
  const [search, setSearch] = useState('');
  const [bloodFilter, setBloodFilter] = useState('');
  const [sortBy, setSortBy] = useState(''); // 'name', 'age'

  // Modals
  const [isAddModalOpen, setIsAddModalOpen] = useState(false);
  const [editingPatient, setEditingPatient] = useState(null);
  const [viewingPatient, setViewingPatient] = useState(null);
  const [patientVisits, setPatientVisits] = useState([]);
  const [deletingId, setDeletingId] = useState(null);
  const [doctors, setDoctors] = useState([]);

  // Form State
  const [formData, setFormData] = useState({
    name: '', age: '', gender: 'Male', contact: '', address: '', bloodGroup: 'O+', notes: ''
  });

  // Visit Note Form State
  const [newVisitNote, setNewVisitNote] = useState({
    doctorId: 'DOC-101', diagnosis: '', prescription: '', notes: ''
  });

  const { addToast } = useToast();

  const loadPatients = async () => {
    try {
      setLoading(true);
      const res = await api.getPatients({
        q: search,
        blood: bloodFilter,
        sort: sortBy
      });
      setPatients(res.data || []);
      const docRes = await api.getDoctors();
      setDoctors(docRes.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load patients', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadPatients();
  }, [search, bloodFilter, sortBy]);

  const handleOpenAdd = () => {
    setFormData({ name: '', age: '', gender: 'Male', contact: '', address: '', bloodGroup: 'O+', notes: '' });
    setIsAddModalOpen(true);
  };

  const handleOpenEdit = (p) => {
    setEditingPatient(p);
    setFormData({
      name: p.name,
      age: p.age,
      gender: p.gender,
      contact: p.contact || '',
      address: p.address || '',
      bloodGroup: p.bloodGroup || 'O+',
      notes: p.notes || ''
    });
  };

  const handleOpenVisits = async (p) => {
    setViewingPatient(p);
    try {
      const res = await api.getPatientVisits(p.id);
      setPatientVisits(res.data || []);
    } catch {
      setPatientVisits([]);
    }
  };

  const handleSavePatient = async (e) => {
    e.preventDefault();
    try {
      if (editingPatient) {
        await api.updatePatient(editingPatient.id, {
          ...formData,
          age: parseInt(formData.age, 10)
        });
        addToast(`Patient ${editingPatient.id} updated successfully!`, 'success');
        setEditingPatient(null);
      } else {
        const res = await api.createPatient({
          ...formData,
          age: parseInt(formData.age, 10)
        });
        addToast(`Patient ${res.data.id} registered successfully!`, 'success');
        setIsAddModalOpen(false);
      }
      loadPatients();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleDeletePatient = async () => {
    if (!deletingId) return;
    try {
      await api.deletePatient(deletingId);
      addToast(`Patient ${deletingId} deleted successfully!`, 'success');
      setDeletingId(null);
      loadPatients();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleAddVisit = async (e) => {
    e.preventDefault();
    if (!viewingPatient) return;
    try {
      const doc = doctors.find((d) => d.id === newVisitNote.doctorId);
      await api.addPatientVisit(viewingPatient.id, {
        ...newVisitNote,
        doctorName: doc ? doc.name : 'Attending Physician',
        visitDate: new Date().toISOString().substring(0, 10)
      });
      addToast('Clinical visit record saved to patient medical history!', 'success');
      setNewVisitNote({ doctorId: 'DOC-101', diagnosis: '', prescription: '', notes: '' });
      const res = await api.getPatientVisits(viewingPatient.id);
      setPatientVisits(res.data || []);
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
              Patient Master Registry
            </h2>
            <span className="badge badge-teal">Medical Records Registry</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Centralized digital registry for hospital inpatient and outpatient medical records.
          </p>
        </div>

        <button type="button" className="btn btn-primary" onClick={handleOpenAdd}>
          <Plus size={16} /> Register New Patient
        </button>
      </div>

      {/* Filter & Search Bar */}
      <div className="card" style={{ padding: '16px 22px', marginBottom: '22px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px', flexWrap: 'wrap' }}>
          
          {/* Search Input */}
          <div style={{ flex: '1', minWidth: '240px', position: 'relative' }}>
            <Search size={16} style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'var(--text-muted)' }} />
            <input
              type="text"
              className="form-input"
              style={{ paddingLeft: '36px' }}
              placeholder="Search by Patient ID (e.g. PAT-1001) or Name..."
              value={search}
              onChange={(e) => setSearch(e.target.value)}
            />
          </div>

          {/* Blood Group Filter */}
          <div style={{ minWidth: '150px' }}>
            <select
              className="form-select"
              value={bloodFilter}
              onChange={(e) => setBloodFilter(e.target.value)}
            >
              <option value="">All Blood Groups</option>
              <option value="A+">A+</option>
              <option value="A-">A-</option>
              <option value="B+">B+</option>
              <option value="B-">B-</option>
              <option value="AB+">AB+</option>
              <option value="AB-">AB-</option>
              <option value="O+">O+</option>
              <option value="O-">O-</option>
            </select>
          </div>

          {/* Sort By Option */}
          <div style={{ minWidth: '170px' }}>
            <select
              className="form-select"
              value={sortBy}
              onChange={(e) => setSortBy(e.target.value)}
            >
              <option value="">Default Order (ID)</option>
              <option value="name">Sort by Name</option>
              <option value="age">Sort by Age</option>
            </select>
          </div>

          {(search || bloodFilter || sortBy) && (
            <button
              type="button"
              className="btn btn-secondary btn-sm"
              onClick={() => { setSearch(''); setBloodFilter(''); setSortBy(''); }}
            >
              Clear Filters
            </button>
          )}

          <div style={{ marginLeft: 'auto', fontSize: '13px', color: 'var(--text-secondary)', fontWeight: 600 }}>
            {patients.length} {patients.length === 1 ? 'Patient' : 'Patients'} Listed
          </div>
        </div>
      </div>

      {/* Patient Table */}
      <div className="card">
        <div className="table-container" style={{ border: 'none' }}>
          <table className="table">
            <thead>
              <tr>
                <th>Patient ID</th>
                <th>Patient Name</th>
                <th>Age / Gender</th>
                <th>Blood Group</th>
                <th>Contact Info</th>
                <th>Registration Date</th>
                <th>Clinical Notes</th>
                <th style={{ textAlign: 'right' }}>Actions</th>
              </tr>
            </thead>
            <tbody>
              {loading ? (
                <tr>
                  <td colSpan="8" style={{ textAlign: 'center', padding: '40px 0', color: 'var(--text-secondary)' }}>
                    Loading patient records...
                  </td>
                </tr>
              ) : patients.length === 0 ? (
                <tr>
                  <td colSpan="8" style={{ textAlign: 'center', padding: '40px 0', color: 'var(--text-secondary)' }}>
                    No patient records found matching criteria.
                  </td>
                </tr>
              ) : (
                patients.map((p) => (
                  <tr key={p.id}>
                    <td>
                      <span style={{
                        fontFamily: 'monospace',
                        fontWeight: 700,
                        color: 'var(--primary-deep)',
                        background: 'var(--mint-tint)',
                        padding: '3px 8px',
                        borderRadius: '6px',
                        border: '1px solid var(--primary-light)',
                        fontSize: '12.5px'
                      }}>
                        {p.id}
                      </span>
                    </td>
                    <td>
                      <div style={{ fontWeight: 700, color: 'var(--text-primary)' }}>{p.name}</div>
                      <div style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>{p.address || 'Address unrecorded'}</div>
                    </td>
                    <td>
                      <span style={{ color: 'var(--text-primary)', fontWeight: 500 }}>
                        {p.age} yrs · {p.gender}
                      </span>
                    </td>
                    <td>
                      <span style={{
                        display: 'inline-block',
                        padding: '2px 8px',
                        borderRadius: '6px',
                        background: '#FBEBEB',
                        color: 'var(--status-danger)',
                        fontWeight: 700,
                        fontSize: '12px',
                        border: '1px solid #F5C6C6'
                      }}>
                        {p.bloodGroup}
                      </span>
                    </td>
                    <td>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '5px', fontSize: '12.5px', color: 'var(--text-primary)' }}>
                        <Phone size={13} style={{ color: 'var(--primary-deep)' }} />
                        <span>{p.contact || 'No phone'}</span>
                      </div>
                    </td>
                    <td>
                      <div style={{ display: 'flex', alignItems: 'center', gap: '5px', fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                        <Calendar size={13} style={{ color: 'var(--text-muted)' }} />
                        <span>{p.registrationDate || '2026-10-09'}</span>
                      </div>
                    </td>
                    <td>
                      <span style={{
                        fontSize: '12.5px',
                        color: 'var(--text-secondary)',
                        maxWidth: '180px',
                        display: 'inline-block',
                        overflow: 'hidden',
                        textOverflow: 'ellipsis',
                        whiteSpace: 'nowrap'
                      }} title={p.notes}>
                        {p.notes || '—'}
                      </span>
                    </td>
                    <td style={{ textAlign: 'right' }}>
                      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'flex-end', gap: '6px' }}>
                        <button
                          type="button"
                          className="btn btn-secondary btn-sm"
                          style={{ padding: '6px 10px' }}
                          onClick={() => handleOpenVisits(p)}
                          title="View Patient Medical History"
                        >
                          <FileText size={14} style={{ color: 'var(--primary-deep)' }} />
                        </button>
                        <button
                          type="button"
                          className="btn btn-secondary btn-sm"
                          style={{ padding: '6px 10px' }}
                          onClick={() => handleOpenEdit(p)}
                          title="Edit Patient Record"
                        >
                          <Edit3 size={14} />
                        </button>
                        <button
                          type="button"
                          className="btn btn-danger-light btn-sm"
                          style={{ padding: '6px 10px' }}
                          onClick={() => setDeletingId(p.id)}
                          title="Delete Patient Record"
                        >
                          <Trash2 size={14} />
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

      {/* MODAL 1: Add / Edit Patient */}
      <Modal
        isOpen={isAddModalOpen || !!editingPatient}
        onClose={() => { setIsAddModalOpen(false); setEditingPatient(null); }}
        title={editingPatient ? `Edit Patient Record (${editingPatient.id})` : 'Register New Patient'}
      >
        <form onSubmit={handleSavePatient}>
          <div className="form-group">
            <label className="form-label">Full Patient Name *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Ramesh Patel"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
            />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr 1fr', gap: '12px' }}>
            <div className="form-group">
              <label className="form-label">Age *</label>
              <input
                type="number"
                required
                min="0"
                max="125"
                className="form-input"
                placeholder="42"
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
              <label className="form-label">Blood Group</label>
              <select
                className="form-select"
                value={formData.bloodGroup}
                onChange={(e) => setFormData({ ...formData, bloodGroup: e.target.value })}
              >
                {['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-'].map((bg) => (
                  <option key={bg} value={bg}>{bg}</option>
                ))}
              </select>
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Contact Phone</label>
            <input
              type="tel"
              className="form-input"
              placeholder="+91 98765 43210"
              value={formData.contact}
              onChange={(e) => setFormData({ ...formData, contact: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Residential Address</label>
            <input
              type="text"
              className="form-input"
              placeholder="Apartment, Street, City"
              value={formData.address}
              onChange={(e) => setFormData({ ...formData, address: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Clinical Notes</label>
            <textarea
              className="form-textarea"
              placeholder="Known medical history, allergies, chronic conditions..."
              value={formData.notes}
              onChange={(e) => setFormData({ ...formData, notes: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '22px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={() => { setIsAddModalOpen(false); setEditingPatient(null); }}
            >
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              {editingPatient ? 'Save Patient Record' : 'Register Patient'}
            </button>
          </div>
        </form>
      </Modal>

      {/* MODAL 2: Patient Visit History */}
      <Modal
        isOpen={!!viewingPatient}
        onClose={() => setViewingPatient(null)}
        title={viewingPatient ? `Medical History — ${viewingPatient.name} (${viewingPatient.id})` : 'Medical History'}
        maxWidth="680px"
      >
        {viewingPatient && (
          <div style={{ display: 'flex', flexDirection: 'column', gap: '18px' }}>
            {/* Patient Header Card */}
            <div style={{
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'space-between',
              background: 'var(--bg-card-sub)',
              padding: '14px 18px',
              borderRadius: '12px',
              border: '1px solid var(--border-soft)'
            }}>
              <div>
                <span style={{ fontSize: '12px', color: 'var(--text-secondary)', textTransform: 'uppercase', fontWeight: 600 }}>
                  Demographics
                </span>
                <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)', marginTop: '2px' }}>
                  {viewingPatient.age} yrs · {viewingPatient.gender} · Blood: {viewingPatient.bloodGroup}
                </div>
              </div>
              <span className="badge badge-teal">Medical History</span>
            </div>

            {/* Existing Visit Records */}
            <div>
              <h4 style={{ fontSize: '14.5px', fontWeight: 700, color: 'var(--text-primary)', marginBottom: '12px' }}>
                Chronological Visit History ({patientVisits.length} Records)
              </h4>

              {patientVisits.length === 0 ? (
                <div style={{ textAlign: 'center', padding: '24px', background: 'var(--bg-canvas)', borderRadius: '10px', color: 'var(--text-secondary)', fontSize: '13px' }}>
                  No prior clinical consultations recorded for this patient.
                </div>
              ) : (
                <div style={{ display: 'flex', flexDirection: 'column', gap: '10px', maxHeight: '240px', overflowY: 'auto' }}>
                  {patientVisits.map((v, i) => (
                    <div
                      key={v.id || i}
                      style={{
                        padding: '12px 16px',
                        background: '#FFFFFF',
                        border: '1px solid var(--border-soft)',
                        borderRadius: '10px',
                        display: 'flex',
                        flexDirection: 'column',
                        gap: '4px'
                      }}
                    >
                      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between' }}>
                        <span style={{ fontWeight: 700, color: 'var(--primary-deep)', fontSize: '13.5px' }}>
                          {v.doctorName || 'Attending Physician'}
                        </span>
                        <span style={{ fontSize: '12px', color: 'var(--text-secondary)', display: 'flex', alignItems: 'center', gap: '4px' }}>
                          <Calendar size={12} style={{ color: 'var(--primary-deep)' }} /> {v.visitDate}
                        </span>
                      </div>
                      <div style={{ fontSize: '13px', color: 'var(--text-primary)' }}>
                        <strong>Diagnosis:</strong> {v.diagnosis || 'General follow-up'}
                      </div>
                      {v.prescription && (
                        <div style={{ fontSize: '12.5px', color: 'var(--primary-deep)', background: 'var(--mint-tint)', padding: '4px 8px', borderRadius: '6px', marginTop: '2px' }}>
                          <strong>Rx:</strong> {v.prescription}
                        </div>
                      )}
                    </div>
                  ))}
                </div>
              )}
            </div>

            {/* Add New Visit Form */}
            <form onSubmit={handleAddVisit} style={{ borderTop: '1px solid var(--border-soft)', paddingTop: '16px' }}>
              <h4 style={{ fontSize: '14px', fontWeight: 700, color: 'var(--text-primary)', marginBottom: '12px' }}>
                Record New Clinical Visit Note
              </h4>

              <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '12px', marginBottom: '12px' }}>
                <div className="form-group" style={{ marginBottom: 0 }}>
                  <label className="form-label">Attending Doctor</label>
                  <select
                    className="form-select"
                    value={newVisitNote.doctorId}
                    onChange={(e) => setNewVisitNote({ ...newVisitNote, doctorId: e.target.value })}
                  >
                    {doctors.map((d) => (
                      <option key={d.id} value={d.id}>{d.name} ({d.department})</option>
                    ))}
                  </select>
                </div>
                <div className="form-group" style={{ marginBottom: 0 }}>
                  <label className="form-label">Diagnosis *</label>
                  <input
                    type="text"
                    required
                    className="form-input"
                    placeholder="e.g. Acute Bronchitis"
                    value={newVisitNote.diagnosis}
                    onChange={(e) => setNewVisitNote({ ...newVisitNote, diagnosis: e.target.value })}
                  />
                </div>
              </div>

              <div className="form-group">
                <label className="form-label">Prescription / Medication</label>
                <input
                  type="text"
                  className="form-input"
                  placeholder="e.g. Amoxicillin 500mg TDS x 5d"
                  value={newVisitNote.prescription}
                  onChange={(e) => setNewVisitNote({ ...newVisitNote, prescription: e.target.value })}
                />
              </div>

              <div className="form-group">
                <label className="form-label">Consultation Notes</label>
                <textarea
                  className="form-textarea"
                  style={{ minHeight: '60px' }}
                  placeholder="Clinical observations, lab test orders, recovery instructions..."
                  value={newVisitNote.notes}
                  onChange={(e) => setNewVisitNote({ ...newVisitNote, notes: e.target.value })}
                />
              </div>

              <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px' }}>
                <button type="button" className="btn btn-secondary btn-sm" onClick={() => setViewingPatient(null)}>
                  Close
                </button>
                <button type="submit" className="btn btn-primary btn-sm">
                  Save Clinical Note
                </button>
              </div>
            </form>
          </div>
        )}
      </Modal>

      {/* CONFIRM DIALOG: Delete Patient */}
      <ConfirmDialog
        isOpen={!!deletingId}
        onClose={() => setDeletingId(null)}
        onConfirm={handleDeletePatient}
        title="Confirm Patient Removal"
        message={`Are you sure you want to delete patient ${deletingId}? This will remove the medical record from the hospital database.`}
        confirmText="Delete Record"
        isDanger={true}
      />
    </div>
  );
}
