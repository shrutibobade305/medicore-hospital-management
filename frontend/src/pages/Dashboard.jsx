import React, { useState, useEffect } from 'react';
import { 
  Users, 
  UserCheck, 
  Calendar, 
  BedDouble, 
  AlertOctagon, 
  Plus, 
  ArrowRight, 
  Zap, 
  Clock, 
  CheckCircle2, 
  Activity,
  Layers,
  HeartPulse,
  Building2,
  TrendingUp,
  ShieldCheck,
  Stethoscope,
  Phone,
  Video,
  FileText,
  ChevronRight
} from 'lucide-react';
import { 
  AreaChart, Area, BarChart, Bar, XAxis, YAxis, CartesianGrid, 
  Tooltip, ResponsiveContainer, Legend 
} from 'recharts';
import { api } from '../services/api';
import StatCard from '../components/StatCard';
import Modal from '../components/Modal';
import { useToast } from '../components/Toast';
import { Link } from 'react-router-dom';
import { getDoctorImage } from '../utils/doctorImages';

export default function Dashboard() {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [modalType, setModalType] = useState(null); // 'patient', 'appointment', 'emergency'
  const { addToast } = useToast();

  // Form states
  const [patientForm, setPatientForm] = useState({
    name: '', age: '', gender: 'Male', contact: '', address: '', bloodGroup: 'O+', notes: ''
  });
  const [apptForm, setApptForm] = useState({
    patientId: '', doctorId: '', appointmentDate: new Date().toISOString().substring(0, 10), appointmentTime: '10:00', reason: '', status: 'Scheduled'
  });
  const [emgForm, setEmgForm] = useState({
    patientName: '', age: '', gender: 'Male', condition: '', severity: 'Critical', notes: ''
  });

  const [patients, setPatients] = useState([]);
  const [doctors, setDoctors] = useState([]);
  const [allAppointments, setAllAppointments] = useState([]);

  const loadDashboard = async () => {
    try {
      setLoading(true);
      const res = await api.getDashboard();
      setData(res.data);
      const pts = await api.getPatients();
      setPatients(pts.data || []);
      const dcs = await api.getDoctors();
      setDoctors(dcs.data || []);
      const appts = await api.getAppointments();
      setAllAppointments(appts.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to load dashboard', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadDashboard();
  }, []);

  const handleDispatchEmergency = async () => {
    try {
      const res = await api.dispatchEmergency();
      addToast(res.message || 'Dispatched emergency case from priority triage queue!', 'success');
      loadDashboard();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleRegisterPatient = async (e) => {
    e.preventDefault();
    try {
      await api.createPatient({ ...patientForm, age: parseInt(patientForm.age, 10) });
      addToast('Patient record registered successfully!', 'success');
      setModalType(null);
      setPatientForm({ name: '', age: '', gender: 'Male', contact: '', address: '', bloodGroup: 'O+', notes: '' });
      loadDashboard();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleBookAppointment = async (e) => {
    e.preventDefault();
    try {
      await api.createAppointment(apptForm);
      addToast('Consultation appointment scheduled successfully!', 'success');
      setModalType(null);
      loadDashboard();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleRegisterEmergency = async (e) => {
    e.preventDefault();
    try {
      await api.createEmergency({ ...emgForm, age: parseInt(emgForm.age || '0', 10) });
      addToast('Emergency case admitted to triage queue!', 'success');
      setModalType(null);
      setEmgForm({ patientName: '', age: '', gender: 'Male', condition: '', severity: 'Critical', notes: '' });
      loadDashboard();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  if (loading && !data) {
    return (
      <div className="page-body" style={{ textAlign: 'center', padding: '120px 0' }}>
        <div style={{
          display: 'inline-flex',
          alignItems: 'center',
          gap: '12px',
          background: '#FFFFFF',
          padding: '16px 28px',
          borderRadius: '16px',
          border: '1px solid var(--border-soft)',
          boxShadow: '0 4px 16px rgba(39, 52, 58, 0.05)'
        }}>
          <HeartPulse size={24} className="animate-spin" style={{ color: 'var(--primary-deep)' }} />
          <span style={{ fontSize: '15px', fontWeight: 600, color: 'var(--text-primary)' }}>
            Loading Hospital Operations Data...
          </span>
        </div>
      </div>
    );
  }

  const d = data || {};

  // Build Real Chart Data: Ward Occupancy from actual backend data
  const wardChartData = (d.wardBreakdown || []).map((w) => ({
    name: w.name,
    Occupied: w.occupiedBeds,
    Available: Math.max(0, w.totalBeds - w.occupiedBeds),
    Total: w.totalBeds
  }));

  // Build Real Chart Data: Appointments Distribution by Department
  const apptByDept = {};
  doctors.forEach((doc) => {
    apptByDept[doc.department] = 0;
  });
  allAppointments.forEach((a) => {
    const doc = doctors.find((d) => d.id === a.doctorId || d.name === a.doctorName);
    const dept = doc ? doc.department : 'General';
    apptByDept[dept] = (apptByDept[dept] || 0) + 1;
  });

  const deptChartData = Object.keys(apptByDept).map((dept) => ({
    department: dept.length > 12 ? dept.substring(0, 10) + '...' : dept,
    Consultations: apptByDept[dept]
  }));

  const pendingEmergencies = d.emergencyQueue || [];
  const recentAppts = d.recentAppointments || [];

  return (
    <div className="page-body">
      
      {/* 1. HEALTH WISE CLINIC HERO BANNER */}
      <div className="hero-banner-grid" style={{
        background: 'linear-gradient(135deg, #4F9F9F 0%, #79BDBD 100%)',
        borderRadius: '18px',
        padding: '32px 36px',
        color: '#FFFFFF',
        marginBottom: '28px',
        position: 'relative',
        overflow: 'hidden',
        boxShadow: '0 8px 24px rgba(79, 159, 159, 0.2)',
        display: 'grid',
        gridTemplateColumns: '1.4fr 1fr',
        alignItems: 'center',
        gap: '30px'
      }}>
        {/* Left Hero Content */}
        <div style={{ zIndex: 2 }}>
          <div style={{
            display: 'inline-flex',
            alignItems: 'center',
            gap: '8px',
            background: 'rgba(255, 255, 255, 0.2)',
            backdropFilter: 'blur(8px)',
            padding: '4px 12px',
            borderRadius: '20px',
            fontSize: '12px',
            fontWeight: 700,
            letterSpacing: '0.04em',
            marginBottom: '14px',
            textTransform: 'uppercase'
          }}>
            <HeartPulse size={14} /> Health Wise Clinic & Hospital Network
          </div>

          <h1 style={{
            fontSize: '32px',
            fontWeight: 800,
            lineHeight: 1.15,
            letterSpacing: '-0.02em',
            marginBottom: '12px',
            color: '#FFFFFF'
          }}>
            Compassionate Care,<br />Proven Wisdom
          </h1>

          <p style={{
            fontSize: '14px',
            lineHeight: 1.6,
            color: '#E6F3F0',
            maxWidth: '520px',
            marginBottom: '22px'
          }}>
            Welcome to the MediCore clinical dashboard. Manage multi-specialty outpatient consultations, surgical beds, emergency triages, and digital medical records across all hospital departments.
          </p>

          <div className="hero-actions" style={{ display: 'flex', gap: '12px', flexWrap: 'wrap' }}>
            <button
              type="button"
              className="btn btn-hero-primary"
              style={{
                background: '#FFFFFF',
                color: 'var(--primary-deep, #2C5E5E)',
                fontWeight: 700,
                padding: '10px 20px',
                borderRadius: '10px',
                boxShadow: '0 4px 12px rgba(39, 52, 58, 0.12)',
                border: 'none',
                cursor: 'pointer'
              }}
              onClick={() => setModalType('appointment')}
            >
              <Calendar size={16} style={{ color: 'var(--primary, #4F9F9F)' }} /> Book Consultation
            </button>
            <button
              type="button"
              className="btn"
              style={{
                background: 'rgba(255, 255, 255, 0.22)',
                color: '#FFFFFF',
                fontWeight: 600,
                border: '1px solid rgba(255, 255, 255, 0.5)',
                padding: '10px 18px',
                borderRadius: '10px'
              }}
              onClick={() => setModalType('patient')}
            >
              <Plus size={16} /> New Patient Intake
            </button>
            <button
              type="button"
              className="btn"
              style={{
                background: '#C95656',
                color: '#FFFFFF',
                fontWeight: 700,
                padding: '10px 18px',
                borderRadius: '10px'
              }}
              onClick={() => setModalType('emergency')}
            >
              <AlertOctagon size={16} /> Emergency Triage
            </button>
          </div>
        </div>

        {/* Right Hero Image Card */}
        <div style={{
          display: 'flex',
          justifyContent: 'center',
          alignItems: 'center',
          position: 'relative'
        }}>
          <div style={{
            position: 'relative',
            width: '260px',
            height: '260px',
            borderRadius: '50%',
            background: 'rgba(255, 255, 255, 0.25)',
            backdropFilter: 'blur(10px)',
            padding: '8px',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center',
            boxShadow: '0 14px 40px rgba(0, 0, 0, 0.2)',
            border: '2px solid rgba(255, 255, 255, 0.5)'
          }}>
            <img
              src="/doctors/doctor-hero-portrait.jpg"
              alt="Chief Medical Specialist"
              style={{
                width: '100%',
                height: '100%',
                objectFit: 'cover',
                borderRadius: '50%',
                border: '4px solid #FFFFFF',
                backgroundColor: '#FFFFFF'
              }}
              onError={(e) => {
                e.target.src = '/doctors/doc-1.png';
              }}
            />
            <div style={{
              position: 'absolute',
              bottom: '10px',
              right: '-6px',
              background: '#FFFFFF',
              color: 'var(--primary-deep, #2C5E5E)',
              padding: '7px 16px',
              borderRadius: '20px',
              fontSize: '12.5px',
              fontWeight: 800,
              boxShadow: '0 6px 18px rgba(39, 52, 58, 0.18)',
              display: 'flex',
              alignItems: 'center',
              gap: '6px',
              zIndex: 3
            }}>
              <CheckCircle2 size={15} style={{ color: 'var(--status-success, #438B68)' }} /> Top Specialist On Duty
            </div>
          </div>
        </div>
      </div>

      {/* 2. STATS OVERVIEW CARDS (6 Metrics) */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))',
        gap: '16px',
        marginBottom: '26px'
      }}>
        <StatCard
          title="Total Registered Patients"
          value={d.totalPatients || 0}
          icon={Users}
          variant="teal"
          subtext="Patient medical registry"
        />
        <StatCard
          title="Active Doctors on Duty"
          value={d.totalDoctors || 0}
          icon={UserCheck}
          variant="aqua"
          subtext="Specialist medical staff"
        />
        <StatCard
          title="Today's Appointments"
          value={d.todayAppointments || 0}
          icon={Calendar}
          variant="purple"
          subtext="Outpatient consultations"
        />
        <StatCard
          title="Available Beds"
          value={d.availableBeds || 0}
          icon={BedDouble}
          variant="success"
          subtext={`${d.totalBeds || 0} total inpatient beds`}
        />
        <StatCard
          title="Bed Occupancy Rate"
          value={`${d.occupancyRate || 0}%`}
          icon={TrendingUp}
          variant="warning"
          subtext={`${d.occupiedBeds || 0} beds occupied`}
        />
        <StatCard
          title="Pending Emergencies"
          value={d.pendingEmergencies || 0}
          icon={AlertOctagon}
          variant="danger"
          subtext="Awaiting triage care"
        />
      </div>

      {/* 3. MEET OUR DOCTORS SHOWCASE */}
      <div className="card" style={{ padding: '24px 28px', marginBottom: '26px' }}>
        <div style={{
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          marginBottom: '20px',
          flexWrap: 'wrap',
          gap: '12px'
        }}>
          <div>
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
              <Stethoscope size={20} style={{ color: 'var(--primary-deep)' }} />
              <h3 style={{ fontSize: '18px', fontWeight: 800, color: 'var(--text-primary)', letterSpacing: '-0.01em' }}>
                Meet our Attending Physicians
              </h3>
            </div>
            <p style={{ fontSize: '13px', color: 'var(--text-secondary)', marginTop: '2px' }}>
              Multi-specialty clinical leaders managing outpatient consultations and surgical care.
            </p>
          </div>

          <Link
            to="/doctors"
            className="btn btn-secondary btn-sm"
            style={{ color: 'var(--primary-deep)', fontWeight: 700 }}
          >
            View Full Staff Directory ({doctors.length}) <ChevronRight size={14} />
          </Link>
        </div>

        {/* Doctor Portraits Row */}
        <div style={{
          display: 'grid',
          gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
          gap: '20px'
        }}>
          {doctors.slice(0, 4).map((doc) => {
            const imgSrc = getDoctorImage(doc.id, doc.name);
            return (
              <div
                key={doc.id}
                style={{
                  background: 'var(--bg-canvas)',
                  border: '1px solid var(--border-soft)',
                  borderRadius: '16px',
                  padding: '18px 16px',
                  textAlign: 'center',
                  display: 'flex',
                  flexDirection: 'column',
                  alignItems: 'center',
                  transition: 'all 0.2s ease',
                  cursor: 'pointer'
                }}
                onMouseEnter={(e) => {
                  e.currentTarget.style.transform = 'translateY(-3px)';
                  e.currentTarget.style.borderColor = 'var(--primary-teal)';
                  e.currentTarget.style.boxShadow = '0 6px 18px rgba(79, 159, 159, 0.12)';
                }}
                onMouseLeave={(e) => {
                  e.currentTarget.style.transform = 'translateY(0)';
                  e.currentTarget.style.borderColor = 'var(--border-soft)';
                  e.currentTarget.style.boxShadow = 'none';
                }}
              >
                {/* Circular Doctor Portrait with Signature Health Wise Ring */}
                <div style={{
                  position: 'relative',
                  width: '90px',
                  height: '90px',
                  borderRadius: '50%',
                  padding: '4px',
                  background: 'linear-gradient(135deg, #79BDBD, #4F9F9F)',
                  marginBottom: '12px',
                  boxShadow: '0 4px 12px rgba(79, 159, 159, 0.2)'
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
                  <span style={{
                    position: 'absolute',
                    bottom: '2px',
                    right: '2px',
                    width: '14px',
                    height: '14px',
                    borderRadius: '50%',
                    background: doc.availability === 'Available' ? 'var(--status-success)' : 'var(--status-warning)',
                    border: '2px solid #FFFFFF'
                  }} title={doc.availability} />
                </div>

                <h4 style={{ fontSize: '15px', fontWeight: 800, color: 'var(--text-primary)', marginBottom: '2px' }}>
                  {doc.name}
                </h4>

                <span style={{
                  fontSize: '12px',
                  fontWeight: 700,
                  color: 'var(--primary-deep)',
                  marginBottom: '6px'
                }}>
                  {doc.department}
                </span>

                <span style={{
                  fontSize: '11px',
                  color: 'var(--text-secondary)',
                  background: '#FFFFFF',
                  padding: '3px 10px',
                  borderRadius: '12px',
                  border: '1px solid var(--border-soft)'
                }}>
                  {doc.roomNo || 'OPD'}
                </span>
              </div>
            );
          })}
        </div>
      </div>

      {/* 4. CHARTS SECTION: Bed Occupancy & Activity by Department */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fit, minmax(420px, 1fr))',
        gap: '24px',
        marginBottom: '26px'
      }}>
        {/* Ward Bed Occupancy Breakdown */}
        <div className="card" style={{ padding: '24px' }}>
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '16px' }}>
            <div>
              <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Ward Bed Capacity & Occupancy
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Real-time bed availability across hospital clinical wards
              </p>
            </div>
            <Link to="/beds" className="badge badge-teal" style={{ textDecoration: 'none' }}>
              Manage Beds <ArrowRight size={12} />
            </Link>
          </div>

          <div style={{ height: '260px', width: '100%' }}>
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={wardChartData} margin={{ top: 10, right: 10, left: -10, bottom: 0 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#E5E7E4" vertical={false} />
                <XAxis dataKey="name" stroke="#707B7F" fontSize={12} tickLine={false} />
                <YAxis stroke="#707B7F" fontSize={12} tickLine={false} allowDecimals={false} />
                <Tooltip
                  contentStyle={{
                    background: '#FFFFFF',
                    border: '1px solid #E5E7E4',
                    borderRadius: '10px',
                    boxShadow: '0 4px 12px rgba(39, 52, 58, 0.08)',
                    fontSize: '12px'
                  }}
                />
                <Legend wrapperStyle={{ fontSize: '12px', paddingTop: '10px' }} />
                <Bar dataKey="Occupied" fill="#4F9F9F" radius={[4, 4, 0, 0]} name="Occupied Beds" />
                <Bar dataKey="Available" fill="#B0DAD8" radius={[4, 4, 0, 0]} name="Available Beds" />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </div>

        {/* Clinical Consultations by Department */}
        <div className="card" style={{ padding: '24px' }}>
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '16px' }}>
            <div>
              <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Departmental Outpatient Load
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Consultation bookings distribution by medical specialty
              </p>
            </div>
            <Link to="/appointments" className="badge badge-teal" style={{ textDecoration: 'none' }}>
              Appointments <ArrowRight size={12} />
            </Link>
          </div>

          <div style={{ height: '260px', width: '100%' }}>
            <ResponsiveContainer width="100%" height="100%">
              <AreaChart data={deptChartData} margin={{ top: 10, right: 10, left: -10, bottom: 0 }}>
                <defs>
                  <linearGradient id="tealGradient" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="5%" stopColor="#4F9F9F" stopOpacity={0.4} />
                    <stop offset="95%" stopColor="#4F9F9F" stopOpacity={0.0} />
                  </linearGradient>
                </defs>
                <CartesianGrid strokeDasharray="3 3" stroke="#E5E7E4" vertical={false} />
                <XAxis dataKey="department" stroke="#707B7F" fontSize={11.5} tickLine={false} />
                <YAxis stroke="#707B7F" fontSize={12} tickLine={false} allowDecimals={false} />
                <Tooltip
                  contentStyle={{
                    background: '#FFFFFF',
                    border: '1px solid #E5E7E4',
                    borderRadius: '10px',
                    boxShadow: '0 4px 12px rgba(39, 52, 58, 0.08)',
                    fontSize: '12px'
                  }}
                />
                <Area
                  type="monotone"
                  dataKey="Consultations"
                  stroke="#4F9F9F"
                  strokeWidth={2.5}
                  fillOpacity={1}
                  fill="url(#tealGradient)"
                  name="Consultations"
                />
              </AreaChart>
            </ResponsiveContainer>
          </div>
        </div>
      </div>

      {/* 5. BOTTOM SPLIT: Emergency Triage Queue + Recent Consultations */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fit, minmax(420px, 1fr))',
        gap: '24px'
      }}>
        {/* Priority Emergency Queue Preview */}
        <div className="card" style={{ display: 'flex', flexDirection: 'column' }}>
          <div className="card-header">
            <div>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <AlertOctagon size={18} style={{ color: 'var(--status-danger)' }} />
                <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                  Priority Triage Queue
                </h3>
              </div>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: '2px' }}>
                Immediate emergency triage ordered by clinical severity
              </p>
            </div>
            <Link to="/emergency" className="badge badge-danger" style={{ textDecoration: 'none' }}>
              Full Triage Board <ArrowRight size={12} />
            </Link>
          </div>

          <div style={{ padding: '0 20px 20px', flex: 1, display: 'flex', flexDirection: 'column' }}>
            {pendingEmergencies.length === 0 ? (
              <div style={{
                textAlign: 'center',
                padding: '40px 20px',
                color: 'var(--text-secondary)',
                fontSize: '13.5px',
                margin: 'auto 0'
              }}>
                <CheckCircle2 size={32} style={{ color: 'var(--status-success)', margin: '0 auto 8px', display: 'block' }} />
                No emergency cases currently pending in triage queue.
              </div>
            ) : (
              <>
                <div style={{ display: 'flex', flexDirection: 'column', gap: '10px', marginBottom: '16px' }}>
                  {pendingEmergencies.slice(0, 3).map((item, idx) => (
                    <div
                      key={item.id}
                      style={{
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'space-between',
                        padding: '12px 14px',
                        background: idx === 0 ? 'var(--mint-tint)' : 'var(--bg-canvas)',
                        borderRadius: '10px',
                        border: idx === 0 ? '1px solid var(--primary-light)' : '1px solid var(--border-soft)'
                      }}
                    >
                      <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
                        <span style={{
                          width: '24px',
                          height: '24px',
                          borderRadius: '50%',
                          background: idx === 0 ? 'var(--primary-deep)' : '#CBD5E1',
                          color: '#FFFFFF',
                          fontSize: '11px',
                          fontWeight: 700,
                          display: 'flex',
                          alignItems: 'center',
                          justifyContent: 'center'
                        }}>
                          {idx + 1}
                        </span>
                        <div>
                          <div style={{ fontWeight: 700, fontSize: '13.5px', color: 'var(--text-primary)' }}>
                            {item.patientName} ({item.age}y)
                          </div>
                          <div style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
                            {item.condition} · Arrived: {item.arrivalTime}
                          </div>
                        </div>
                      </div>

                      <span className={`badge ${item.severity === 'Critical' ? 'badge-danger' : item.severity === 'High' ? 'badge-warning' : 'badge-teal'}`}>
                        {item.severity}
                      </span>
                    </div>
                  ))}
                </div>

                <div style={{ marginTop: 'auto', display: 'flex', gap: '10px' }}>
                  <button
                    type="button"
                    className="btn btn-primary btn-sm"
                    style={{ flex: 1 }}
                    onClick={handleDispatchEmergency}
                  >
                    <Zap size={14} /> Dispatch Next Emergency Patient
                  </button>
                  <button
                    type="button"
                    className="btn btn-secondary btn-sm"
                    onClick={() => setModalType('emergency')}
                  >
                    <Plus size={14} /> Intake Case
                  </button>
                </div>
              </>
            )}
          </div>
        </div>

        {/* Recent Outpatient Consultations */}
        <div className="card" style={{ display: 'flex', flexDirection: 'column' }}>
          <div className="card-header">
            <div>
              <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                <Clock size={18} style={{ color: 'var(--primary-deep)' }} />
                <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                  Recent Consultations
                </h3>
              </div>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)', marginTop: '2px' }}>
                Latest consultation bookings processed through outpatient scheduling
              </p>
            </div>
            <Link to="/appointments" className="badge badge-teal" style={{ textDecoration: 'none' }}>
              View All <ArrowRight size={12} />
            </Link>
          </div>

          <div className="table-container" style={{ border: 'none', margin: '0 10px 10px' }}>
            <table className="table">
              <thead>
                <tr>
                  <th>Patient</th>
                  <th>Physician</th>
                  <th>Time</th>
                  <th>Status</th>
                </tr>
              </thead>
              <tbody>
                {recentAppts.length === 0 ? (
                  <tr>
                    <td colSpan="4" style={{ textAlign: 'center', padding: '30px 0', color: 'var(--text-secondary)' }}>
                      No recent consultations recorded.
                    </td>
                  </tr>
                ) : (
                  recentAppts.slice(0, 4).map((a) => (
                    <tr key={a.id}>
                      <td style={{ fontWeight: 700, color: 'var(--text-primary)' }}>
                        {a.patientName}
                      </td>
                      <td style={{ color: 'var(--text-secondary)' }}>
                        {a.doctorName}
                      </td>
                      <td style={{ fontSize: '12px', color: 'var(--text-secondary)' }}>
                        {a.appointmentDate} · {a.appointmentTime}
                      </td>
                      <td>
                        <span className={`badge ${a.status === 'Completed' ? 'badge-available' : a.status === 'Waiting' ? 'badge-warning' : 'badge-teal'}`}>
                          {a.status}
                        </span>
                      </td>
                    </tr>
                  ))
                )}
              </tbody>
            </table>
          </div>
        </div>
      </div>

      {/* MODALS */}
      {/* 1. New Patient Modal */}
      <Modal
        isOpen={modalType === 'patient'}
        onClose={() => setModalType(null)}
        title="New Patient Registration"
      >
        <form onSubmit={handleRegisterPatient}>
          <div className="form-group">
            <label className="form-label">Full Name *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Ramesh Patel"
              value={patientForm.name}
              onChange={(e) => setPatientForm({ ...patientForm, name: e.target.value })}
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
                value={patientForm.age}
                onChange={(e) => setPatientForm({ ...patientForm, age: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Gender</label>
              <select
                className="form-select"
                value={patientForm.gender}
                onChange={(e) => setPatientForm({ ...patientForm, gender: e.target.value })}
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
                value={patientForm.bloodGroup}
                onChange={(e) => setPatientForm({ ...patientForm, bloodGroup: e.target.value })}
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
              value={patientForm.contact}
              onChange={(e) => setPatientForm({ ...patientForm, contact: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Residential Address</label>
            <input
              type="text"
              className="form-input"
              placeholder="Apartment, Street, City"
              value={patientForm.address}
              onChange={(e) => setPatientForm({ ...patientForm, address: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Clinical Notes</label>
            <textarea
              className="form-textarea"
              placeholder="Initial medical observations or known allergies..."
              value={patientForm.notes}
              onChange={(e) => setPatientForm({ ...patientForm, notes: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
            <button type="button" className="btn btn-secondary" onClick={() => setModalType(null)}>
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              Register Patient
            </button>
          </div>
        </form>
      </Modal>

      {/* 2. Book Appointment Modal */}
      <Modal
        isOpen={modalType === 'appointment'}
        onClose={() => setModalType(null)}
        title="Schedule Outpatient Consultation"
      >
        <form onSubmit={handleBookAppointment}>
          <div className="form-group">
            <label className="form-label">Select Patient *</label>
            <select
              required
              className="form-select"
              value={apptForm.patientId}
              onChange={(e) => setApptForm({ ...apptForm, patientId: e.target.value })}
            >
              <option value="">-- Choose Patient --</option>
              {patients.map((p) => (
                <option key={p.id} value={p.id}>{p.name} ({p.id} · Age {p.age})</option>
              ))}
            </select>
          </div>

          <div className="form-group">
            <label className="form-label">Select Doctor *</label>
            <select
              required
              className="form-select"
              value={apptForm.doctorId}
              onChange={(e) => setApptForm({ ...apptForm, doctorId: e.target.value })}
            >
              <option value="">-- Choose Physician --</option>
              {doctors.map((doc) => (
                <option key={doc.id} value={doc.id}>{doc.name} ({doc.department} · {doc.roomNo})</option>
              ))}
            </select>
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '16px' }}>
            <div className="form-group">
              <label className="form-label">Appointment Date *</label>
              <input
                type="date"
                required
                className="form-input"
                value={apptForm.appointmentDate}
                onChange={(e) => setApptForm({ ...apptForm, appointmentDate: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Time Slot *</label>
              <input
                type="time"
                required
                className="form-input"
                value={apptForm.appointmentTime}
                onChange={(e) => setApptForm({ ...apptForm, appointmentTime: e.target.value })}
              />
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Consultation Reason / Symptoms</label>
            <input
              type="text"
              className="form-input"
              placeholder="e.g. Routine hypertension checkup, follow-up"
              value={apptForm.reason}
              onChange={(e) => setApptForm({ ...apptForm, reason: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
            <button type="button" className="btn btn-secondary" onClick={() => setModalType(null)}>
              Cancel
            </button>
            <button type="submit" className="btn btn-primary">
              Confirm Consultation
            </button>
          </div>
        </form>
      </Modal>

      {/* 3. Emergency Intake Modal */}
      <Modal
        isOpen={modalType === 'emergency'}
        onClose={() => setModalType(null)}
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
              value={emgForm.patientName}
              onChange={(e) => setEmgForm({ ...emgForm, patientName: e.target.value })}
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
                value={emgForm.age}
                onChange={(e) => setEmgForm({ ...emgForm, age: e.target.value })}
              />
            </div>
            <div className="form-group">
              <label className="form-label">Gender</label>
              <select
                className="form-select"
                value={emgForm.gender}
                onChange={(e) => setEmgForm({ ...emgForm, gender: e.target.value })}
              >
                <option value="Male">Male</option>
                <option value="Female">Female</option>
                <option value="Other">Other</option>
              </select>
            </div>
            <div className="form-group">
              <label className="form-label">Acuity Severity *</label>
              <select
                required
                className="form-select"
                value={emgForm.severity}
                onChange={(e) => setEmgForm({ ...emgForm, severity: e.target.value })}
              >
                <option value="Critical">Critical (Priority 1)</option>
                <option value="High">High (Priority 2)</option>
                <option value="Medium">Medium (Priority 3)</option>
                <option value="Low">Low (Priority 4)</option>
              </select>
            </div>
          </div>

          <div className="form-group">
            <label className="form-label">Diagnosis / Acute Condition *</label>
            <input
              type="text"
              required
              className="form-input"
              placeholder="e.g. Acute Myocardial Infarction, Severe Head Trauma"
              value={emgForm.condition}
              onChange={(e) => setEmgForm({ ...emgForm, condition: e.target.value })}
            />
          </div>

          <div className="form-group">
            <label className="form-label">Triage & Resuscitation Notes</label>
            <textarea
              className="form-textarea"
              placeholder="Vital signs, GCS score, oxygen saturation, trauma notes..."
              value={emgForm.notes}
              onChange={(e) => setEmgForm({ ...emgForm, notes: e.target.value })}
            />
          </div>

          <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
            <button type="button" className="btn btn-secondary" onClick={() => setModalType(null)}>
              Cancel
            </button>
            <button type="submit" className="btn btn-danger">
              Intake Emergency Patient
            </button>
          </div>
        </form>
      </Modal>

    </div>
  );
}
