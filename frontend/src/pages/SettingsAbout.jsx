import React, { useState, useEffect } from 'react';
import { 
  Building, 
  Clock, 
  Mail, 
  Phone, 
  MapPin, 
  ShieldCheck, 
  Calendar, 
  Bell, 
  Users, 
  HardDrive, 
  Save, 
  CheckCircle2, 
  RotateCcw, 
  Download, 
  Server, 
  Stethoscope, 
  Plus, 
  Edit3, 
  Trash2,
  Lock,
  FileSpreadsheet
} from 'lucide-react';
import { api } from '../services/api';
import { useToast } from '../components/Toast';
import Modal from '../components/Modal';

export default function HospitalSettings() {
  const [activeTab, setActiveTab] = useState('profile');
  const [loading, setLoading] = useState(false);
  const [resetting, setResetting] = useState(false);
  const [health, setHealth] = useState(null);
  const [stats, setStats] = useState(null);
  const { addToast } = useToast();

  // Hospital Profile State
  const [profile, setProfile] = useState({
    hospitalName: 'MediCore Multi-Specialty Hospital & Research Center',
    tagline: 'Compassionate Care, Proven Clinical Wisdom',
    licenseNo: 'NABH-JCI-BLR-2026-8849',
    address: 'Plot 42, Health City Avenue, Medical Enclave, Bengaluru, Karnataka 560068',
    phone: '+91 80 4968 2000',
    emergencyPhone: '+91 80 4968 2001 / 108',
    email: 'contact@medicore.org',
    emergencyEmail: 'emergency@medicore.org',
    website: 'https://medicore.healthcare.org',
    operatingHours: '24x7 Inpatient & Emergency Services | Outpatient OPD: 08:00 AM – 08:00 PM',
    bedCapacity: '150 Inpatient Beds'
  });

  // Appointment Configuration State
  const [apptSettings, setApptSettings] = useState({
    slotDuration: '30',
    advanceBookingDays: '30',
    cancellationWindowHours: '2',
    enableAutoCheckin: true,
    enableConflictPrevention: true,
    enableSmsReminders: true,
    enableEmailConfirmations: true
  });

  // Notification Preferences State
  const [notifications, setNotifications] = useState({
    emergencyAudioAlert: true,
    bedThresholdAlert: true,
    bedThresholdPercentage: '85',
    dailySummaryEmail: false,
    staffDutyReminders: true
  });

  // Departments List
  const [departments, setDepartments] = useState([
    { id: 'DEP-CARD', name: 'Cardiology & Cardiac Care', head: 'Dr. Rajesh Rao', room: 'OPD-101', phone: '+91 80 4968 2101', beds: 20 },
    { id: 'DEP-NEUR', name: 'Neurology & Neurovascular', head: 'Dr. Shalini Gupta', room: 'OPD-102', phone: '+91 80 4968 2102', beds: 16 },
    { id: 'DEP-ORTH', name: 'Orthopedics & Joint Trauma', head: 'Dr. Amitava Roy', room: 'OPD-103', phone: '+91 80 4968 2103', beds: 24 },
    { id: 'DEP-OBGY', name: 'Obstetrics & Gynecology', head: 'Dr. Preethi Hegde', room: 'OPD-104', phone: '+91 80 4968 2104', beds: 18 },
    { id: 'DEP-EMRG', name: 'Emergency Medicine & Resuscitation', head: 'Dr. Farhan Siddiqui', room: 'EMG-Bay 1', phone: '+91 80 4968 2105', beds: 12 },
    { id: 'DEP-PULM', name: 'Pulmonology & Critical Care', head: 'Dr. Kavita Menon', room: 'OPD-105', phone: '+91 80 4968 2106', beds: 14 }
  ]);

  const [deptModalOpen, setDeptModalOpen] = useState(false);
  const [deptForm, setDeptForm] = useState({ id: '', name: '', head: '', room: '', phone: '', beds: 10 });

  const checkStatus = async () => {
    try {
      setLoading(true);
      const [hRes, sRes] = await Promise.all([api.getHealth(), api.getDashboard()]);
      setHealth(hRes);
      setStats(sRes);
    } catch (err) {
      setHealth({ status: 'offline', error: err.message });
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    checkStatus();
  }, []);

  const handleSaveProfile = (e) => {
    e.preventDefault();
    addToast('Hospital profile and facility information updated!', 'success');
  };

  const handleSaveApptSettings = (e) => {
    e.preventDefault();
    addToast('Appointment scheduling rules and policies updated!', 'success');
  };

  const handleSaveNotifications = (e) => {
    e.preventDefault();
    addToast('Notification preferences and alert thresholds updated!', 'success');
  };

  const handleSaveDepartment = (e) => {
    e.preventDefault();
    if (deptForm.id && departments.some(d => d.id === deptForm.id)) {
      setDepartments(departments.map(d => d.id === deptForm.id ? deptForm : d));
      addToast(`Department ${deptForm.name} updated!`, 'success');
    } else {
      const newId = `DEP-${Math.random().toString(36).substring(2, 6).toUpperCase()}`;
      setDepartments([...departments, { ...deptForm, id: newId }]);
      addToast(`New department ${deptForm.name} registered!`, 'success');
    }
    setDeptModalOpen(false);
    setDeptForm({ id: '', name: '', head: '', room: '', phone: '', beds: 10 });
  };

  const handleReset = async () => {
    if (!window.confirm('Are you sure you want to reset the system database? All records will revert to their original clinical demo state.')) {
      return;
    }
    setResetting(true);
    try {
      await api.resetDatabase();
      addToast('Database successfully reset and reseeded with clinical demo records!', 'success');
      checkStatus();
    } catch (err) {
      addToast(err.message || 'Failed to reset database', 'error');
    } finally {
      setResetting(false);
    }
  };

  const handleExportFullBackup = async () => {
    try {
      const [pts, dcs, apts, emg, act] = await Promise.all([
        api.getPatients(),
        api.getDoctors(),
        api.getAppointments(),
        api.getAllEmergencies(),
        api.getActivity()
      ]);

      const backup = {
        hospital: profile.hospitalName,
        exportDate: new Date().toISOString(),
        patients: pts.data || [],
        doctors: dcs.data || [],
        appointments: apts.data || [],
        emergencies: emg.data || [],
        auditLog: act.data || []
      };

      const dataStr = 'data:text/json;charset=utf-8,' + encodeURIComponent(JSON.stringify(backup, null, 2));
      const downloadAnchor = document.createElement('a');
      downloadAnchor.setAttribute('href', dataStr);
      downloadAnchor.setAttribute('download', `medicore-hospital-backup-${new Date().toISOString().substring(0, 10)}.json`);
      document.body.appendChild(downloadAnchor);
      downloadAnchor.click();
      downloadAnchor.remove();
      addToast('Complete hospital backup exported successfully (.json)!', 'success');
    } catch (err) {
      addToast(err.message || 'Export failed', 'error');
    }
  };

  // Staff roles matrix
  const staffRoles = [
    {
      role: 'Hospital Administrator',
      level: 'Super Admin',
      permissions: ['Full Access', 'User & Staff Management', 'System & Policy Configuration', 'Database Maintenance', 'Financial & Operational Audit'],
      usersCount: 2
    },
    {
      role: 'Chief Medical Officer (CMO)',
      level: 'Executive Clinical',
      permissions: ['Clinical Oversight', 'Triage Override', 'All Medical Records', 'Staff Rostering', 'Clinical Incident Reports'],
      usersCount: 3
    },
    {
      role: 'Attending Physician / Specialist',
      level: 'Medical Practitioner',
      permissions: ['Patient Consultations', 'Diagnostic & Rx Entry', 'Patient Medical History', 'OPD Schedule Management'],
      usersCount: 18
    },
    {
      role: 'Admissions & Reception Clerk',
      level: 'Front Desk Operations',
      permissions: ['Patient Registration', 'Appointment Booking', 'Inpatient Ward Check-in', 'Discharge Invoicing'],
      usersCount: 8
    },
    {
      role: 'Nursing & Triage Officer',
      level: 'Clinical Care Support',
      permissions: ['Triage Acuity Assessment', 'Vital Signs Entry', 'Bed Status Updates', 'Medication Tracking'],
      usersCount: 24
    }
  ];

  return (
    <div className="page-body">
      {/* Page Header */}
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
              Hospital Settings
            </h2>
            <span className="badge badge-teal">Administration & Facility Controls</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Configure hospital facility details, medical departments, appointment scheduling policies, and data management.
          </p>
        </div>

        <div style={{ display: 'flex', gap: '10px' }}>
          <button
            type="button"
            className="btn btn-secondary"
            onClick={handleExportFullBackup}
          >
            <Download size={15} style={{ color: 'var(--primary-deep)' }} /> Export Full Backup (.json)
          </button>
        </div>
      </div>

      {/* Settings Navigation Tabs */}
      <div style={{
        display: 'flex',
        borderBottom: '1px solid var(--border-soft)',
        marginBottom: '24px',
        overflowX: 'auto',
        gap: '4px'
      }}>
        {[
          { id: 'profile', label: 'Hospital Profile', icon: Building },
          { id: 'departments', label: 'Departments', icon: Stethoscope },
          { id: 'appointments', label: 'Appointment Rules', icon: Calendar },
          { id: 'notifications', label: 'Notifications & Alerts', icon: Bell },
          { id: 'roles', label: 'Roles & Permissions', icon: Users },
          { id: 'data', label: 'Data Management', icon: HardDrive },
        ].map((tab) => {
          const Icon = tab.icon;
          const isActive = activeTab === tab.id;
          return (
            <button
              key={tab.id}
              type="button"
              onClick={() => setActiveTab(tab.id)}
              style={{
                padding: '12px 20px',
                fontSize: '13.5px',
                fontWeight: 700,
                borderBottom: isActive ? '2px solid var(--primary-deep)' : '2px solid transparent',
                color: isActive ? 'var(--primary-deep)' : 'var(--text-secondary)',
                background: isActive ? 'var(--mint-tint)' : 'transparent',
                borderTopLeftRadius: '8px',
                borderTopRightRadius: '8px',
                cursor: 'pointer',
                borderTop: 'none',
                borderLeft: 'none',
                borderRight: 'none',
                display: 'flex',
                alignItems: 'center',
                gap: '8px',
                whiteSpace: 'nowrap',
                transition: 'all 0.15s ease'
              }}
            >
              <Icon size={16} /> {tab.label}
            </button>
          );
        })}
      </div>

      {/* TAB 1: Hospital Profile */}
      {activeTab === 'profile' && (
        <div className="card" style={{ padding: '28px 32px' }}>
          <div style={{ marginBottom: '22px' }}>
            <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)' }}>
              Hospital Facility Profile
            </h3>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginTop: '2px' }}>
              Official medical center identification, registration licenses, and contact information.
            </p>
          </div>

          <form onSubmit={handleSaveProfile}>
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(280px, 1fr))', gap: '20px' }}>
              <div className="form-group">
                <label className="form-label">Hospital / Institution Name *</label>
                <input
                  type="text"
                  required
                  className="form-input"
                  value={profile.hospitalName}
                  onChange={(e) => setProfile({ ...profile, hospitalName: e.target.value })}
                />
              </div>

              <div className="form-group">
                <label className="form-label">Healthcare Motto / Tagline</label>
                <input
                  type="text"
                  className="form-input"
                  value={profile.tagline}
                  onChange={(e) => setProfile({ ...profile, tagline: e.target.value })}
                />
              </div>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(280px, 1fr))', gap: '20px' }}>
              <div className="form-group">
                <label className="form-label">Accreditation & License Number</label>
                <input
                  type="text"
                  className="form-input"
                  value={profile.licenseNo}
                  onChange={(e) => setProfile({ ...profile, licenseNo: e.target.value })}
                />
              </div>

              <div className="form-group">
                <label className="form-label">Official Website</label>
                <input
                  type="url"
                  className="form-input"
                  value={profile.website}
                  onChange={(e) => setProfile({ ...profile, website: e.target.value })}
                />
              </div>
            </div>

            <div className="form-group">
              <label className="form-label">Physical Campus Address *</label>
              <input
                type="text"
                required
                className="form-input"
                value={profile.address}
                onChange={(e) => setProfile({ ...profile, address: e.target.value })}
              />
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(240px, 1fr))', gap: '20px' }}>
              <div className="form-group">
                <label className="form-label">Primary Reception Phone</label>
                <input
                  type="text"
                  className="form-input"
                  value={profile.phone}
                  onChange={(e) => setProfile({ ...profile, phone: e.target.value })}
                />
              </div>

              <div className="form-group">
                <label className="form-label">24/7 Emergency Hotline *</label>
                <input
                  type="text"
                  required
                  className="form-input"
                  value={profile.emergencyPhone}
                  onChange={(e) => setProfile({ ...profile, emergencyPhone: e.target.value })}
                />
              </div>

              <div className="form-group">
                <label className="form-label">Administrative Email</label>
                <input
                  type="email"
                  className="form-input"
                  value={profile.email}
                  onChange={(e) => setProfile({ ...profile, email: e.target.value })}
                />
              </div>
            </div>

            <div className="form-group">
              <label className="form-label">Operating Hours & Schedule</label>
              <input
                type="text"
                className="form-input"
                value={profile.operatingHours}
                onChange={(e) => setProfile({ ...profile, operatingHours: e.target.value })}
              />
            </div>

            <div style={{ display: 'flex', justifyContent: 'flex-end', marginTop: '10px' }}>
              <button type="submit" className="btn btn-primary">
                <Save size={15} /> Save Facility Profile
              </button>
            </div>
          </form>
        </div>
      )}

      {/* TAB 2: Clinical Departments */}
      {activeTab === 'departments' && (
        <div className="card">
          <div className="card-header">
            <div>
              <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)' }}>
                Hospital Clinical Departments
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Active medical divisions, department heads, and consultation facilities
              </p>
            </div>
            <button
              type="button"
              className="btn btn-primary btn-sm"
              onClick={() => {
                setDeptForm({ id: '', name: '', head: '', room: '', phone: '', beds: 10 });
                setDeptModalOpen(true);
              }}
            >
              <Plus size={14} /> Add Department
            </button>
          </div>

          <div className="table-container" style={{ border: 'none' }}>
            <table className="table">
              <thead>
                <tr>
                  <th>Department Code</th>
                  <th>Clinical Department Name</th>
                  <th>Head of Department</th>
                  <th>OPD / Ward Facility</th>
                  <th>Direct Contact</th>
                  <th style={{ textAlign: 'right' }}>Actions</th>
                </tr>
              </thead>
              <tbody>
                {departments.map((dept) => (
                  <tr key={dept.id}>
                    <td>
                      <span style={{ fontFamily: 'monospace', fontWeight: 700, color: 'var(--primary-deep)', fontSize: '12px' }}>
                        {dept.id}
                      </span>
                    </td>
                    <td>
                      <div style={{ fontWeight: 700, color: 'var(--text-primary)' }}>{dept.name}</div>
                    </td>
                    <td>
                      <div style={{ color: 'var(--text-primary)', fontWeight: 600 }}>{dept.head}</div>
                    </td>
                    <td>
                      <span className="badge badge-teal">{dept.room}</span>
                    </td>
                    <td style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                      {dept.phone}
                    </td>
                    <td style={{ textAlign: 'right' }}>
                      <button
                        type="button"
                        className="btn btn-secondary btn-sm"
                        onClick={() => {
                          setDeptForm(dept);
                          setDeptModalOpen(true);
                        }}
                      >
                        <Edit3 size={13} /> Edit
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* TAB 3: Appointment Rules */}
      {activeTab === 'appointments' && (
        <div className="card" style={{ padding: '28px 32px' }}>
          <div style={{ marginBottom: '22px' }}>
            <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)' }}>
              Outpatient Consultation & Scheduling Rules
            </h3>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginTop: '2px' }}>
              Define consultation slot lengths, booking windows, cancellation policies, and patient arrival handling.
            </p>
          </div>

          <form onSubmit={handleSaveApptSettings}>
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(260px, 1fr))', gap: '20px' }}>
              <div className="form-group">
                <label className="form-label">Default Consultation Duration</label>
                <select
                  className="form-select"
                  value={apptSettings.slotDuration}
                  onChange={(e) => setApptSettings({ ...apptSettings, slotDuration: e.target.value })}
                >
                  <option value="15">15 Minutes (Express Follow-up)</option>
                  <option value="20">20 Minutes</option>
                  <option value="30">30 Minutes (Standard Clinical Consult)</option>
                  <option value="45">45 Minutes (Detailed Examination)</option>
                  <option value="60">60 Minutes (Specialist Comprehensive)</option>
                </select>
              </div>

              <div className="form-group">
                <label className="form-label">Advance Booking Window</label>
                <select
                  className="form-select"
                  value={apptSettings.advanceBookingDays}
                  onChange={(e) => setApptSettings({ ...apptSettings, advanceBookingDays: e.target.value })}
                >
                  <option value="7">7 Days in Advance</option>
                  <option value="14">14 Days in Advance</option>
                  <option value="30">30 Days in Advance (Recommended)</option>
                  <option value="60">60 Days in Advance</option>
                </select>
              </div>

              <div className="form-group">
                <label className="form-label">Free Cancellation Policy</label>
                <select
                  className="form-select"
                  value={apptSettings.cancellationWindowHours}
                  onChange={(e) => setApptSettings({ ...apptSettings, cancellationWindowHours: e.target.value })}
                >
                  <option value="1">Up to 1 Hour before slot</option>
                  <option value="2">Up to 2 Hours before slot (Standard)</option>
                  <option value="12">Up to 12 Hours before slot</option>
                  <option value="24">Up to 24 Hours before slot</option>
                </select>
              </div>
            </div>

            <div style={{ marginTop: '16px', display: 'flex', flexDirection: 'column', gap: '14px' }}>
              <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '13.5px', color: 'var(--text-primary)', cursor: 'pointer' }}>
                <input
                  type="checkbox"
                  checked={apptSettings.enableConflictPrevention}
                  onChange={(e) => setApptSettings({ ...apptSettings, enableConflictPrevention: e.target.checked })}
                />
                <strong>Enforce Strict Physician Conflict Prevention:</strong> Prevent double-booking doctors for the same time slot across all hospital wards.
              </label>

              <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '13.5px', color: 'var(--text-primary)', cursor: 'pointer' }}>
                <input
                  type="checkbox"
                  checked={apptSettings.enableAutoCheckin}
                  onChange={(e) => setApptSettings({ ...apptSettings, enableAutoCheckin: e.target.checked })}
                />
                <strong>Auto-Arrival Check-in:</strong> Place patients into the live consultation waiting queue upon check-in at reception.
              </label>

              <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '13.5px', color: 'var(--text-primary)', cursor: 'pointer' }}>
                <input
                  type="checkbox"
                  checked={apptSettings.enableSmsReminders}
                  onChange={(e) => setApptSettings({ ...apptSettings, enableSmsReminders: e.target.checked })}
                />
                <strong>Automated Patient SMS Reminders:</strong> Dispatch appointment confirmations and slot reminders to patients.
              </label>
            </div>

            <div style={{ display: 'flex', justifyContent: 'flex-end', marginTop: '24px' }}>
              <button type="submit" className="btn btn-primary">
                <Save size={15} /> Save Scheduling Rules
              </button>
            </div>
          </form>
        </div>
      )}

      {/* TAB 4: Notifications & Alerts */}
      {activeTab === 'notifications' && (
        <div className="card" style={{ padding: '28px 32px' }}>
          <div style={{ marginBottom: '22px' }}>
            <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)' }}>
              Hospital Notifications & Critical Alert Preferences
            </h3>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginTop: '2px' }}>
              Control emergency triage sirens, bed capacity warnings, and hospital-wide operational broadcasts.
            </p>
          </div>

          <form onSubmit={handleSaveNotifications}>
            <div style={{ display: 'flex', flexDirection: 'column', gap: '18px' }}>
              <div style={{
                padding: '16px 20px',
                background: 'var(--bg-card-sub)',
                border: '1px solid var(--border-soft)',
                borderRadius: '12px'
              }}>
                <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '14px', color: 'var(--text-primary)', cursor: 'pointer' }}>
                  <input
                    type="checkbox"
                    checked={notifications.emergencyAudioAlert}
                    onChange={(e) => setNotifications({ ...notifications, emergencyAudioAlert: e.target.checked })}
                  />
                  <strong>Critical Emergency Triage Alerts:</strong> Play visual & audio alerts on dashboard whenever a Priority 1 case arrives.
                </label>
              </div>

              <div style={{
                padding: '16px 20px',
                background: 'var(--bg-card-sub)',
                border: '1px solid var(--border-soft)',
                borderRadius: '12px'
              }}>
                <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '14px', color: 'var(--text-primary)', cursor: 'pointer', marginBottom: '10px' }}>
                  <input
                    type="checkbox"
                    checked={notifications.bedThresholdAlert}
                    onChange={(e) => setNotifications({ ...notifications, bedThresholdAlert: e.target.checked })}
                  />
                  <strong>Inpatient Ward Bed Capacity Warning:</strong> Alert hospital administration when overall bed occupancy exceeds safety threshold.
                </label>
                
                <div style={{ display: 'flex', alignItems: 'center', gap: '12px', marginLeft: '24px' }}>
                  <span style={{ fontSize: '13px', color: 'var(--text-secondary)' }}>Capacity Trigger:</span>
                  <select
                    className="form-select"
                    style={{ width: '140px' }}
                    value={notifications.bedThresholdPercentage}
                    onChange={(e) => setNotifications({ ...notifications, bedThresholdPercentage: e.target.value })}
                  >
                    <option value="75">75% Capacity</option>
                    <option value="80">80% Capacity</option>
                    <option value="85">85% Capacity (Standard)</option>
                    <option value="90">90% Capacity</option>
                    <option value="95">95% Critical Surge</option>
                  </select>
                </div>
              </div>

              <div style={{
                padding: '16px 20px',
                background: 'var(--bg-card-sub)',
                border: '1px solid var(--border-soft)',
                borderRadius: '12px'
              }}>
                <label style={{ display: 'flex', alignItems: 'center', gap: '10px', fontSize: '14px', color: 'var(--text-primary)', cursor: 'pointer' }}>
                  <input
                    type="checkbox"
                    checked={notifications.staffDutyReminders}
                    onChange={(e) => setNotifications({ ...notifications, staffDutyReminders: e.target.checked })}
                  />
                  <strong>Physician Shift & Consultation Handover Reminders:</strong> Automatic notifications for OPD shift starts and bed discharges.
                </label>
              </div>
            </div>

            <div style={{ display: 'flex', justifyContent: 'flex-end', marginTop: '24px' }}>
              <button type="submit" className="btn btn-primary">
                <Save size={15} /> Save Notification Preferences
              </button>
            </div>
          </form>
        </div>
      )}

      {/* TAB 5: Roles & Access Permissions */}
      {activeTab === 'roles' && (
        <div className="card">
          <div className="card-header">
            <div>
              <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)' }}>
                Staff Roles & Access Control Policy
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Configured permissions for clinical, administrative, and nursing personnel
              </p>
            </div>
          </div>

          <div className="table-container" style={{ border: 'none' }}>
            <table className="table">
              <thead>
                <tr>
                  <th>Hospital Staff Role</th>
                  <th>Access Tier</th>
                  <th>Granted Clinical & Administrative Permissions</th>
                  <th>Active Personnel</th>
                </tr>
              </thead>
              <tbody>
                {staffRoles.map((role, idx) => (
                  <tr key={idx}>
                    <td>
                      <div style={{ fontWeight: 700, color: 'var(--text-primary)' }}>{role.role}</div>
                    </td>
                    <td>
                      <span className="badge badge-teal">{role.level}</span>
                    </td>
                    <td>
                      <div style={{ display: 'flex', flexWrap: 'wrap', gap: '6px' }}>
                        {role.permissions.map((p, i) => (
                          <span
                            key={i}
                            style={{
                              background: '#FFFFFF',
                              border: '1px solid var(--border-soft)',
                              borderRadius: '6px',
                              padding: '2px 8px',
                              fontSize: '11.5px',
                              color: 'var(--text-primary)'
                            }}
                          >
                            ✓ {p}
                          </span>
                        ))}
                      </div>
                    </td>
                    <td>
                      <strong style={{ color: 'var(--primary-deep)' }}>{role.usersCount}</strong> staff
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>
      )}

      {/* TAB 6: Data Management */}
      {activeTab === 'data' && (
        <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
          
          {/* Status & Connection Card */}
          <div className="card" style={{ padding: '24px 28px' }}>
            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '16px' }}>
              <div>
                <h3 style={{ fontSize: '17px', fontWeight: 800, color: 'var(--text-primary)' }}>
                  Hospital Relational Database Status
                </h3>
                <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                  SQLite persistent database engine with PRAGMA foreign key constraints enabled.
                </p>
              </div>
              <span className={`badge ${health?.status === 'ok' || health?.status === 'online' ? 'badge-available' : 'badge-danger'}`}>
                {health?.status === 'ok' || health?.status === 'online' ? 'All Systems Operational' : 'Offline'}
              </span>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(220px, 1fr))', gap: '16px' }}>
              <div style={{ background: 'var(--bg-card-sub)', padding: '14px 18px', borderRadius: '10px', border: '1px solid var(--border-soft)' }}>
                <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)', textTransform: 'uppercase', fontWeight: 600 }}>Database File</div>
                <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)', marginTop: '2px' }}>data/hospital.db</div>
              </div>
              <div style={{ background: 'var(--bg-card-sub)', padding: '14px 18px', borderRadius: '10px', border: '1px solid var(--border-soft)' }}>
                <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)', textTransform: 'uppercase', fontWeight: 600 }}>Total Patients</div>
                <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)', marginTop: '2px' }}>{stats?.totalPatients || 0} Records</div>
              </div>
              <div style={{ background: 'var(--bg-card-sub)', padding: '14px 18px', borderRadius: '10px', border: '1px solid var(--border-soft)' }}>
                <div style={{ fontSize: '11.5px', color: 'var(--text-secondary)', textTransform: 'uppercase', fontWeight: 600 }}>Active Physicians</div>
                <div style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)', marginTop: '2px' }}>{stats?.totalDoctors || 0} Staff</div>
              </div>
            </div>
          </div>

          {/* Backup & Export */}
          <div className="card" style={{ padding: '24px 28px' }}>
            <h4 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)', marginBottom: '6px' }}>
              Export Clinical Records Backup
            </h4>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginBottom: '16px' }}>
              Export a complete structured JSON archive of all patient files, physician schedules, consultations, emergency admissions, and audit logs.
            </p>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={handleExportFullBackup}
            >
              <Download size={15} style={{ color: 'var(--primary-deep)' }} /> Export Complete Hospital Backup (.json)
            </button>
          </div>

          {/* Database Reseeding & Demo Maintenance */}
          <div className="card" style={{ padding: '24px 28px' }}>
            <h4 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)', marginBottom: '6px' }}>
              Database Reset & Reseeding
            </h4>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginBottom: '16px' }}>
              Revert the hospital database back to its default demonstration baseline with pre-populated patient profiles, on-duty doctors, ward bed allotments, and consultation records.
            </p>
            <button
              type="button"
              onClick={handleReset}
              disabled={resetting}
              className="btn btn-secondary"
              style={{ color: 'var(--status-danger)', borderColor: '#FECACA', background: '#FEF2F2' }}
            >
              <RotateCcw size={15} className={resetting ? 'animate-spin' : ''} />
              {resetting ? 'Resetting Database...' : 'Reset to Default Hospital Demo Data'}
            </button>
          </div>

        </div>
      )}

      {/* MODAL: Add / Edit Department */}
      <Modal
        isOpen={deptModalOpen}
        onClose={() => setDeptModalOpen(false)}
        title={deptForm.id ? `Edit Department (${deptForm.name})` : 'Register New Clinical Department'}
      >
        <form onSubmit={handleSaveDepartment}>
          <div className="form-group">
            <label className="form-label">Department Name *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Dermatology & Cosmetology"
              value={deptForm.name}
              onChange={(e) => setDeptForm({ ...deptForm, name: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Head of Department (Physician)</label>
            <input
              type="text"
              className="form-input"
              placeholder="e.g. Dr. Priya Sharma"
              value={deptForm.head}
              onChange={(e) => setDeptForm({ ...deptForm, head: e.target.value })}
            />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '16px' }}>
            <div className="form-group">
              <label className="form-label">OPD Room / Location</label>
              <input
                type="text"
                className="form-input"
                placeholder="OPD-201"
                value={deptForm.room}
                onChange={(e) => setDeptForm({ ...deptForm, room: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Direct Contact Phone</label>
              <input
                type="text"
                className="form-input"
                placeholder="+91 80 4968 2109"
                value={deptForm.phone}
                onChange={(e) => setDeptForm({ ...deptForm, phone: e.target.value })}
              />
            </div>
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
            <button
              type="button"
              className="btn btn-secondary"
              onClick={() => setDeptModalOpen(false)}
            >
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              Save Department
            </button>
          </div>
        </form>
      </Modal>

    </div>
  );
}
