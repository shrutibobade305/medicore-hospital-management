import React, { useState, useEffect } from 'react';
import { 
  UserCheck, 
  Search, 
  Plus, 
  Edit3, 
  Trash2, 
  Calendar, 
  Clock, 
  Mail, 
  Phone, 
  MapPin, 
  ArrowUpDown,
  Stethoscope,
  Building,
  CheckCircle2,
  XCircle,
  Eye,
  Layers
} from 'lucide-react';
import { api } from '../services/api';
import Modal from '../components/Modal';
import ConfirmDialog from '../components/ConfirmDialog';
import { useToast } from '../components/Toast';
import { getDoctorImage } from '../utils/doctorImages';

export default function Doctors() {
  const [doctors, setDoctors] = useState([]);
  const [loading, setLoading] = useState(true);
  const [search, setSearch] = useState('');
  const [deptFilter, setDeptFilter] = useState('');
  const [isSortedMode, setIsSortedMode] = useState(false);

  // Modals
  const [isAddModalOpen, setIsAddModalOpen] = useState(false);
  const [editingDoctor, setEditingDoctor] = useState(null);
  const [selectedDoctorAppts, setSelectedDoctorAppts] = useState(null);
  const [deletingId, setDeletingId] = useState(null);
  const [doctorAppointments, setDoctorAppointments] = useState([]);

  const { addToast } = useToast();

  const [formData, setFormData] = useState({
    name: '', department: 'Cardiology', specialization: '', contact: '', email: '', availability: 'Available', roomNo: 'OPD-101'
  });

  const loadDoctors = async () => {
    try {
      setLoading(true);
      let res;
      if (isSortedMode) {
        res = await api.getDoctorsSorted();
      } else {
        res = await api.getDoctors({ dept: deptFilter, q: search });
      }
      setDoctors(res.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load doctors', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadDoctors();
  }, [search, deptFilter, isSortedMode]);

  const handleOpenAdd = () => {
    setFormData({ name: '', department: 'Cardiology', specialization: '', contact: '', email: '', availability: 'Available', roomNo: 'OPD-101' });
    setIsAddModalOpen(true);
  };

  const handleOpenEdit = (doc) => {
    setEditingDoctor(doc);
    setFormData(doc);
  };

  const handleViewSchedule = async (doc) => {
    setSelectedDoctorAppts(doc);
    try {
      const res = await api.getAppointments({ doctorId: doc.id });
      setDoctorAppointments(res.data || []);
    } catch {
      setDoctorAppointments([]);
    }
  };

  const handleSaveDoctor = async (e) => {
    e.preventDefault();
    try {
      if (editingDoctor) {
        await api.updateDoctor(editingDoctor.id, formData);
        addToast(`Doctor ${editingDoctor.id} profile updated successfully!`, 'success');
        setEditingDoctor(null);
      } else {
        const res = await api.createDoctor(formData);
        addToast(`Doctor ${res.data.id} added to medical staff!`, 'success');
        setIsAddModalOpen(false);
      }
      loadDoctors();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleDeleteDoctor = async () => {
    if (!deletingId) return;
    try {
      await api.deleteDoctor(deletingId);
      addToast(`Doctor ${deletingId} removed from staff directory.`, 'success');
      setDeletingId(null);
      loadDoctors();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const departments = Array.from(new Set(doctors.map((d) => d.department)));

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
              Physicians & Medical Staff Directory
            </h2>
            <span className="badge badge-teal">Medical Staff Directory</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Hospital physician roster, clinical specialties, and outpatient consultation schedules.
          </p>
        </div>

        <div style={{ display: 'flex', gap: '10px' }}>
          <button
            type="button"
            className={`btn ${isSortedMode ? 'btn-primary' : 'btn-secondary'}`}
            onClick={() => setIsSortedMode(!isSortedMode)}
            title="Sort staff alphabetically by name"
          >
            <ArrowUpDown size={16} />
            {isSortedMode ? 'Alphabetical Order Active' : 'Sort Alphabetically'}
          </button>

          <button type="button" className="btn btn-primary" onClick={handleOpenAdd}>
            <Plus size={16} /> Add Physician
          </button>
        </div>
      </div>

      {/* Filter Bar */}
      <div className="card" style={{ padding: '16px 22px', marginBottom: '22px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px', flexWrap: 'wrap' }}>
          
          <div style={{ flex: '1', minWidth: '240px', position: 'relative' }}>
            <Search size={16} style={{ position: 'absolute', left: '12px', top: '50%', transform: 'translateY(-50%)', color: 'var(--text-muted)' }} />
            <input
              type="text"
              className="form-input"
              style={{ paddingLeft: '36px' }}
              placeholder="Search by Doctor ID or Physician Name..."
              value={search}
              disabled={isSortedMode}
              onChange={(e) => setSearch(e.target.value)}
            />
          </div>

          <div style={{ minWidth: '180px' }}>
            <select
              className="form-select"
              value={deptFilter}
              disabled={isSortedMode}
              onChange={(e) => setDeptFilter(e.target.value)}
            >
              <option value="">All Departments</option>
              {departments.map((dept) => (
                <option key={dept} value={dept}>{dept}</option>
              ))}
            </select>
          </div>

          {(search || deptFilter) && (
            <button
              type="button"
              className="btn btn-secondary btn-sm"
              onClick={() => { setSearch(''); setDeptFilter(''); }}
            >
              Clear Filters
            </button>
          )}

          <div style={{ marginLeft: 'auto', fontSize: '13px', color: 'var(--text-secondary)', fontWeight: 600 }}>
            {doctors.length} Physicians on Staff
          </div>
        </div>
      </div>

      {/* Doctor Cards Grid */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fill, minmax(340px, 1fr))',
        gap: '20px'
      }}>
        {loading ? (
          <div className="card" style={{ padding: '40px', textAlign: 'center', gridColumn: '1 / -1', color: 'var(--text-secondary)' }}>
            Loading physician staff directory...
          </div>
        ) : doctors.length === 0 ? (
          <div className="card" style={{ padding: '40px', textAlign: 'center', gridColumn: '1 / -1', color: 'var(--text-secondary)' }}>
            No physician records found matching criteria.
          </div>
        ) : (
          doctors.map((doc) => {
            const imgSrc = getDoctorImage(doc.id, doc.name);
            return (
              <div key={doc.id} className="card" style={{ padding: '22px 24px', display: 'flex', flexDirection: 'column', justifyContent: 'space-between' }}>
                <div>
                  {/* Top Row: Doctor Photo, Name & Availability */}
                  <div style={{ display: 'flex', alignItems: 'flex-start', justifyContent: 'space-between', marginBottom: '14px' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '14px' }}>
                      {/* Signature Health Wise Circular Ring Frame */}
                      <div style={{
                        width: '54px',
                        height: '54px',
                        borderRadius: '50%',
                        padding: '3px',
                        background: 'linear-gradient(135deg, var(--primary-light), var(--primary-deep))',
                        boxShadow: '0 2px 8px rgba(79, 159, 159, 0.2)',
                        flexShrink: 0
                      }}>
                        <img
                          src={imgSrc}
                          alt={doc.name}
                          style={{
                            width: '100%',
                            height: '100%',
                            objectFit: 'cover',
                            borderRadius: '50%',
                            border: '2px solid #FFFFFF',
                            backgroundColor: '#FFFFFF'
                          }}
                          onError={(e) => {
                            e.target.src = '/doctors/doc-1.png';
                          }}
                        />
                      </div>
                      <div>
                        <h3 style={{ fontSize: '16px', fontWeight: 800, color: 'var(--text-primary)', lineHeight: 1.2 }}>
                          {doc.name}
                        </h3>
                        <span style={{
                          fontFamily: 'monospace',
                          fontSize: '11.5px',
                          fontWeight: 700,
                          color: 'var(--primary-deep)'
                        }}>
                          {doc.id}
                        </span>
                      </div>
                    </div>

                    <span className={`badge ${doc.availability === 'Available' ? 'badge-available' : 'badge-maintenance'}`}>
                      {doc.availability === 'Available' ? 'On Duty' : 'Off Duty'}
                    </span>
                  </div>

                  {/* Specialization & Department */}
                  <div style={{
                    background: 'var(--bg-card-sub)',
                    padding: '10px 14px',
                    borderRadius: '10px',
                    border: '1px solid var(--border-soft)',
                    marginBottom: '14px'
                  }}>
                    <div style={{ fontSize: '13px', fontWeight: 700, color: 'var(--primary-deep)' }}>
                      {doc.department}
                    </div>
                    <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginTop: '2px' }}>
                      {doc.specialization || 'Clinical Specialist'}
                    </div>
                  </div>

                  {/* Contact & Room Details */}
                  <div style={{ display: 'flex', flexDirection: 'column', gap: '6px', fontSize: '12.5px', color: 'var(--text-primary)', marginBottom: '18px' }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <Building size={14} style={{ color: 'var(--primary-deep)', flexShrink: 0 }} />
                      <span>Consultation: <strong>{doc.roomNo || 'OPD-101'}</strong></span>
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <Phone size={14} style={{ color: 'var(--text-secondary)', flexShrink: 0 }} />
                      <span>{doc.contact || '+91 98765 00000'}</span>
                    </div>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                      <Mail size={14} style={{ color: 'var(--text-secondary)', flexShrink: 0 }} />
                      <span style={{ overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' }}>
                        {doc.email || `${doc.id.toLowerCase()}@medicore.org`}
                      </span>
                    </div>
                  </div>
                </div>

                {/* Action Buttons */}
                <div style={{
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  borderTop: '1px solid var(--border-soft)',
                  paddingTop: '14px',
                  marginTop: 'auto'
                }}>
                  <button
                    type="button"
                    className="btn btn-secondary btn-sm"
                    style={{ color: 'var(--primary-deep)' }}
                    onClick={() => handleViewSchedule(doc)}
                  >
                    <Calendar size={13} /> View Schedule
                  </button>

                  <div style={{ display: 'flex', gap: '6px' }}>
                    <button
                      type="button"
                      className="btn btn-secondary btn-sm"
                      style={{ padding: '6px 10px' }}
                      onClick={() => handleOpenEdit(doc)}
                      title="Edit Doctor Details"
                    >
                      <Edit3 size={13} />
                    </button>
                    <button
                      type="button"
                      className="btn btn-danger-light btn-sm"
                      style={{ padding: '6px 10px' }}
                      onClick={() => setDeletingId(doc.id)}
                      title="Remove Physician Record"
                    >
                      <Trash2 size={13} />
                    </button>
                  </div>
                </div>
              </div>
            );
          })
        )}
      </div>

      {/* MODAL: Add / Edit Doctor */}
      <Modal
        isOpen={isAddModalOpen || !!editingDoctor}
        onClose={() => { setIsAddModalOpen(false); setEditingDoctor(null); }}
        title={editingDoctor ? `Edit Physician (${editingDoctor.id})` : 'Add New Physician'}
      >
        <form onSubmit={handleSaveDoctor}>
          <div className="form-group">
            <label className="form-label">Doctor Name *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Dr. Ananya Sen"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
            />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '16px' }}>
            <div className="form-group">
              <label className="form-label">Department *</label>
              <select
                className="form-select"
                value={formData.department}
                onChange={(e) => setFormData({ ...formData, department: e.target.value })}
              >
                <option value="Cardiology">Cardiology</option>
                <option value="Neurology">Neurology</option>
                <option value="Orthopedics">Orthopedics</option>
                <option value="Obstetrics & Gyn">Obstetrics & Gyn</option>
                <option value="General Surgery">General Surgery</option>
                <option value="Emergency Medicine">Emergency Medicine</option>
                <option value="Internal Medicine">Internal Medicine</option>
                <option value="Pulmonology">Pulmonology</option>
              </select>
            </div>
            <div className="form-group">
              <label className="form-label">Availability Status</label>
              <select
                className="form-select"
                value={formData.availability}
                onChange={(e) => setFormData({ ...formData, availability: e.target.value })}
              >
                <option value="Available">Available / On Duty</option>
                <option value="On Leave">On Leave / Off Duty</option>
              </select>
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Specialization / Sub-discipline</label>
            <input
              type="text"
              className="form-input"
              placeholder="e.g. Interventional Cardiology, Pediatric Critical Care"
              value={formData.specialization}
              onChange={(e) => setFormData({ ...formData, specialization: e.target.value })}
            />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1.2fr 1fr', gap: '16px' }}>
            <div className="form-group">
              <label className="form-label">Contact Phone</label>
              <input
                type="tel"
                className="form-input"
                placeholder="+91 98765 11111"
                value={formData.contact}
                onChange={(e) => setFormData({ ...formData, contact: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">OPD Room Number</label>
              <input
                type="text"
                className="form-input"
                placeholder="OPD-204"
                value={formData.roomNo}
                onChange={(e) => setFormData({ ...formData, roomNo: e.target.value })}
              />
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Email Address</label>
            <input
              type="email"
              className="form-input"
              placeholder="doctor@medicore.org"
              value={formData.email}
              onChange={(e) => setFormData({ ...formData, email: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '22px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={() => { setIsAddModalOpen(false); setEditingDoctor(null); }}
            >
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              {editingDoctor ? 'Save Physician Profile' : 'Add Physician'}
            </button>
          </div>
        </form>
      </Modal>

      {/* MODAL: Doctor Consultations Schedule */}
      <Modal
        isOpen={!!selectedDoctorAppts}
        onClose={() => setSelectedDoctorAppts(null)}
        title={selectedDoctorAppts ? `Consultation Schedule — ${selectedDoctorAppts.name} (${selectedDoctorAppts.id})` : 'Schedule'}
        maxWidth="640px"
      >
        {selectedDoctorAppts && (
          <div>
            <div style={{
              background: 'var(--bg-card-sub)',
              padding: '12px 18px',
              borderRadius: '12px',
              border: '1px solid var(--border-soft)',
              marginBottom: '16px',
              display: 'flex',
              justifyContent: 'space-between',
              alignItems: 'center'
            }}>
              <div>
                <span style={{ fontSize: '13px', fontWeight: 700, color: 'var(--text-primary)' }}>
                  {selectedDoctorAppts.department} · {selectedDoctorAppts.roomNo}
                </span>
                <p style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
                  {doctorAppointments.length} Active Consultations Booked
                </p>
              </div>
              <span className="badge badge-teal">Consultation Schedule</span>
            </div>

            {doctorAppointments.length === 0 ? (
              <div style={{ textAlign: 'center', padding: '30px', color: 'var(--text-secondary)', background: 'var(--bg-canvas)', borderRadius: '10px' }}>
                No active appointments booked for this physician.
              </div>
            ) : (
              <div style={{ display: 'flex', flexDirection: 'column', gap: '10px', maxHeight: '320px', overflowY: 'auto' }}>
                {doctorAppointments.map((a) => (
                  <div
                    key={a.id}
                    style={{
                      padding: '12px 16px',
                      background: '#FFFFFF',
                      border: '1px solid var(--border-soft)',
                      borderRadius: '10px',
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'space-between'
                    }}
                  >
                    <div>
                      <div style={{ fontWeight: 700, fontSize: '14px', color: 'var(--text-primary)' }}>
                        {a.patientName}
                      </div>
                      <div style={{ fontSize: '12px', color: 'var(--text-secondary)', display: 'flex', alignItems: 'center', gap: '4px', marginTop: '2px' }}>
                        <Clock size={12} style={{ color: 'var(--primary-deep)' }} />
                        {a.appointmentDate} at {a.appointmentTime} · {a.reason || 'General Consult'}
                      </div>
                    </div>

                    <span className={`badge ${a.status === 'Completed' ? 'badge-available' : a.status === 'Waiting' ? 'badge-high' : 'badge-info'}`}>
                      {a.status}
                    </span>
                  </div>
                ))}
              </div>
            )}

            <div style={{ display: 'flex', justifyContent: 'flex-end', marginTop: '20px' }}>
              <button type="button" className="btn btn-secondary btn-sm" onClick={() => setSelectedDoctorAppts(null)}>
                Close Schedule
              </button>
            </div>
          </div>
        )}
      </Modal>

      {/* CONFIRM DIALOG: Delete Doctor */}
      <ConfirmDialog
        isOpen={!!deletingId}
        onClose={() => setDeletingId(null)}
        onConfirm={handleDeleteDoctor}
        title="Confirm Doctor Removal"
        message={`Are you sure you want to remove doctor ${deletingId}? This will remove the physician from the active hospital directory.`}
        confirmText="Remove Physician"
        isDanger={true}
      />
    </div>
  );
}
