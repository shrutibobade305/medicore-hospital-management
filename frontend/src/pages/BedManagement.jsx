import React, { useState, useEffect } from 'react';
import { 
  BedDouble, 
  RotateCcw, 
  Plus, 
  CheckCircle2, 
  XCircle, 
  AlertTriangle, 
  UserCheck, 
  Layers, 
  LogOut,
  Building,
  Filter,
  User,
  ShieldCheck,
  Check
} from 'lucide-react';
import { api } from '../services/api';
import Modal from '../components/Modal';
import { useToast } from '../components/Toast';

export default function BedManagement() {
  const [beds, setBeds] = useState([]);
  const [wards, setWards] = useState([]);
  const [patients, setPatients] = useState([]);
  const [loading, setLoading] = useState(true);

  // Filters
  const [selectedWard, setSelectedWard] = useState('');
  const [selectedStatus, setSelectedStatus] = useState('');

  // Modals
  const [allocatingBed, setAllocatingBed] = useState(null);
  const [selectedPatientId, setSelectedPatientId] = useState('');
  const [undoing, setUndoing] = useState(false);

  const { addToast } = useToast();

  const loadData = async () => {
    try {
      setLoading(true);
      const bedRes = await api.getBeds({
        wardId: selectedWard,
        status: selectedStatus
      });
      setBeds(bedRes.data || []);

      const wardRes = await api.getWards();
      setWards(wardRes.data || []);

      const ptRes = await api.getPatients();
      setPatients(ptRes.data || []);
    } catch (err) {
      addToast(err.message || 'Failed to fetch bed data', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadData();
  }, [selectedWard, selectedStatus]);

  const handleAllocateBed = async (e) => {
    e.preventDefault();
    if (!allocatingBed || !selectedPatientId) return;
    try {
      await api.allocateBed(allocatingBed.id, selectedPatientId);
      addToast(`Bed ${allocatingBed.id} allocated! Recorded in Undo Stack.`, 'success');
      setAllocatingBed(null);
      setSelectedPatientId('');
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleReleaseBed = async (bedId) => {
    if (!window.confirm(`Discharge patient and release bed ${bedId}? Action can be undone via Undo Stack.`)) return;
    try {
      await api.releaseBed(bedId);
      addToast(`Bed ${bedId} released! Recorded in Undo Stack.`, 'success');
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleUndo = async () => {
    try {
      setUndoing(true);
      const res = await api.undoBedAction();
      addToast(res.message || 'Undone recent bed action via Custom Stack!', 'success');
      loadData();
    } catch (err) {
      addToast(err.message, 'error');
    } finally {
      setUndoing(false);
    }
  };

  // Metrics
  const totalBeds = beds.length;
  const availableBeds = beds.filter((b) => b.status === 'Available').length;
  const occupiedBeds = beds.filter((b) => b.status === 'Occupied').length;
  const occupancyRate = totalBeds > 0 ? Math.round((occupiedBeds / totalBeds) * 100) : 0;

  // Group beds by ward
  const bedsByWard = {};
  wards.forEach((w) => {
    bedsByWard[w.id] = { ward: w, beds: [] };
  });
  beds.forEach((b) => {
    if (bedsByWard[b.wardId]) {
      bedsByWard[b.wardId].beds.push(b);
    } else {
      bedsByWard[b.wardId] = { ward: { id: b.wardId, name: b.wardName || b.wardId }, beds: [b] };
    }
  });

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
              Bed & Ward Management
            </h2>
            <span className="badge badge-teal">Inpatient Wards & Beds</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Real-time inpatient bed allocation, patient admissions, and ward capacity management.
          </p>
        </div>

        <button
          type="button"
          className="btn btn-secondary"
          onClick={handleUndo}
          disabled={undoing}
          style={{ color: 'var(--primary-deep)', borderColor: 'var(--primary-light)', background: 'var(--mint-tint)' }}
          title="Revert the last bed allocation or discharge transaction"
        >
          <RotateCcw size={16} className={undoing ? 'animate-spin' : ''} />
          Undo Last Bed Action
        </button>
      </div>

      {/* Summary KPI Cards */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))',
        gap: '16px',
        marginBottom: '24px'
      }}>
        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid #4F9F9F' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: '#707B7F', textTransform: 'uppercase' }}>
            Total Registered Beds
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: '#27343A', marginTop: '2px' }}>
            {totalBeds}
          </div>
          <div style={{ fontSize: '11.5px', color: '#707B7F' }}>Across {wards.length} clinical wards</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid #438B68' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: '#438B68', textTransform: 'uppercase' }}>
            Available Beds
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: '#438B68', marginTop: '2px' }}>
            {availableBeds}
          </div>
          <div style={{ fontSize: '11.5px', color: '#707B7F' }}>Ready for admission</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid #C95656' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: '#C95656', textTransform: 'uppercase' }}>
            Occupied Beds
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: '#C95656', marginTop: '2px' }}>
            {occupiedBeds}
          </div>
          <div style={{ fontSize: '11.5px', color: '#707B7F' }}>Current inpatient count</div>
        </div>

        <div className="card" style={{ padding: '16px 20px', borderLeft: '4px solid #D9A441' }}>
          <div style={{ fontSize: '12px', fontWeight: 700, color: '#B88528', textTransform: 'uppercase' }}>
            Occupancy Rate
          </div>
          <div style={{ fontSize: '28px', fontWeight: 800, color: '#27343A', marginTop: '2px' }}>
            {occupancyRate}%
          </div>
          <div style={{ fontSize: '11.5px', color: '#707B7F' }}>Hospital census load</div>
        </div>
      </div>

      {/* Filter Toolbar */}
      <div className="card" style={{ padding: '16px 22px', marginBottom: '24px' }}>
        <div style={{ display: 'flex', alignItems: 'center', gap: '16px', flexWrap: 'wrap' }}>
          
          <div style={{ minWidth: '200px' }}>
            <label style={{ fontSize: '11px', fontWeight: 700, textTransform: 'uppercase', color: '#707B7F', display: 'block', marginBottom: '4px' }}>
              Filter by Ward
            </label>
            <select
              className="form-select"
              value={selectedWard}
              onChange={(e) => setSelectedWard(e.target.value)}
            >
              <option value="">All Hospital Wards</option>
              {wards.map((w) => (
                <option key={w.id} value={w.id}>{w.name}</option>
              ))}
            </select>
          </div>

          <div style={{ minWidth: '180px' }}>
            <label style={{ fontSize: '11px', fontWeight: 700, textTransform: 'uppercase', color: '#707B7F', display: 'block', marginBottom: '4px' }}>
              Filter by Status
            </label>
            <select
              className="form-select"
              value={selectedStatus}
              onChange={(e) => setSelectedStatus(e.target.value)}
            >
              <option value="">All Bed Statuses</option>
              <option value="Available">Available (Green)</option>
              <option value="Occupied">Occupied (Red)</option>
              <option value="Maintenance">Maintenance (Grey)</option>
            </select>
          </div>

          {(selectedWard || selectedStatus) && (
            <div style={{ alignSelf: 'flex-end' }}>
              <button
                type="button"
                className="btn btn-secondary btn-sm"
                onClick={() => { setSelectedWard(''); setSelectedStatus(''); }}
              >
                Reset Filters
              </button>
            </div>
          )}

          <div style={{ marginLeft: 'auto', alignSelf: 'flex-end', display: 'flex', alignItems: 'center', gap: '16px', fontSize: '12px', color: '#707B7F' }}>
            <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
              <span style={{ width: '10px', height: '10px', borderRadius: '50%', background: '#438B68' }}></span> Available
            </span>
            <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
              <span style={{ width: '10px', height: '10px', borderRadius: '50%', background: '#C95656' }}></span> Occupied
            </span>
            <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
              <span style={{ width: '10px', height: '10px', borderRadius: '50%', background: '#9DA7AA' }}></span> Maintenance
            </span>
          </div>
        </div>
      </div>

      {/* Bed Grid Grouped by Ward */}
      <div style={{ display: 'flex', flexDirection: 'column', gap: '26px' }}>
        {loading ? (
          <div className="card" style={{ padding: '50px', textAlign: 'center', color: '#707B7F' }}>
            Loading hospital ward layout...
          </div>
        ) : Object.keys(bedsByWard).length === 0 ? (
          <div className="card" style={{ padding: '50px', textAlign: 'center', color: '#707B7F' }}>
            No beds found matching filter criteria.
          </div>
        ) : (
          Object.values(bedsByWard)
            .filter((group) => group.beds.length > 0)
            .map(({ ward, beds: wardBeds }) => {
              const wardOccupied = wardBeds.filter((b) => b.status === 'Occupied').length;
              const wardTotal = wardBeds.length;
              const pct = wardTotal > 0 ? Math.round((wardOccupied / wardTotal) * 100) : 0;

              return (
                <div key={ward.id} className="card" style={{ overflow: 'hidden' }}>
                  {/* Ward Header Banner */}
                  <div style={{
                    padding: '16px 24px',
                    background: '#FAF8F5',
                    borderBottom: '1px solid #E5E7E4',
                    display: 'flex',
                    alignItems: 'center',
                    justifyContent: 'space-between',
                    flexWrap: 'wrap',
                    gap: '10px'
                  }}>
                    <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
                      <Building size={18} style={{ color: '#4F9F9F' }} />
                      <h3 style={{ fontSize: '16px', fontWeight: 700, color: '#27343A' }}>
                        {ward.name}
                      </h3>
                      <span style={{ fontSize: '12px', color: '#707B7F', fontFamily: 'monospace' }}>
                        ({ward.id})
                      </span>
                    </div>

                    <div style={{ display: 'flex', alignItems: 'center', gap: '14px', fontSize: '13px' }}>
                      <span style={{ color: '#707B7F' }}>
                        Occupancy: <strong>{wardOccupied} / {wardTotal} Beds</strong> ({pct}%)
                      </span>
                      <div style={{
                        width: '100px',
                        height: '7px',
                        background: '#E5E7E4',
                        borderRadius: '9999px',
                        overflow: 'hidden'
                      }}>
                        <div style={{
                          height: '100%',
                          width: `${pct}%`,
                          background: pct > 80 ? '#C95656' : pct > 50 ? '#D9A441' : '#438B68',
                          borderRadius: '9999px'
                        }} />
                      </div>
                    </div>
                  </div>

                  {/* Beds Matrix in this Ward */}
                  <div style={{
                    padding: '22px 24px',
                    display: 'grid',
                    gridTemplateColumns: 'repeat(auto-fill, minmax(240px, 1fr))',
                    gap: '16px'
                  }}>
                    {wardBeds.map((bed) => {
                      const isAvailable = bed.status === 'Available';
                      const isOccupied = bed.status === 'Occupied';

                      return (
                        <div
                          key={bed.id}
                          style={{
                            padding: '16px',
                            borderRadius: '12px',
                            border: `1px solid ${isAvailable ? '#C4E5D4' : isOccupied ? '#F5C6C6' : '#E5E7E4'}`,
                            background: isAvailable ? '#EBF5F0' : isOccupied ? '#FBEBEB' : '#FAF8F5',
                            display: 'flex',
                            flexDirection: 'column',
                            justifyContent: 'space-between',
                            gap: '12px',
                            transition: 'all 0.15s ease'
                          }}
                        >
                          <div>
                            <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '6px' }}>
                              <span style={{ fontFamily: 'monospace', fontWeight: 700, fontSize: '14px', color: '#27343A' }}>
                                {bed.id}
                              </span>
                              <span className={`badge ${isAvailable ? 'badge-available' : isOccupied ? 'badge-occupied' : 'badge-maintenance'}`}>
                                {bed.status}
                              </span>
                            </div>

                            <div style={{ fontSize: '12px', color: '#707B7F' }}>
                              Bed Type: <strong>{bed.type || 'Standard'}</strong>
                            </div>

                            {isOccupied && (
                              <div style={{
                                marginTop: '10px',
                                padding: '8px 10px',
                                background: '#FFFFFF',
                                borderRadius: '8px',
                                border: '1px solid #F5C6C6',
                                fontSize: '12.5px'
                              }}>
                                <div style={{ color: '#707B7F', fontSize: '11px', textTransform: 'uppercase' }}>Patient:</div>
                                <div style={{ fontWeight: 700, color: '#27343A' }}>{bed.patientName || 'Admitted Patient'}</div>
                                <div style={{ fontSize: '11px', color: '#707B7F' }}>ID: {bed.patientId}</div>
                              </div>
                            )}

                            {isAvailable && (
                              <div style={{ marginTop: '10px', fontSize: '12px', color: '#438B68', fontWeight: 500 }}>
                                ✓ Ready for patient intake
                              </div>
                            )}
                          </div>

                          {/* Action */}
                          <div style={{ paddingTop: '8px', borderTop: '1px dashed rgba(0,0,0,0.08)' }}>
                            {isAvailable ? (
                              <button
                                type="button"
                                className="btn btn-primary btn-sm"
                                style={{ width: '100%', fontSize: '12.5px' }}
                                onClick={() => setAllocatingBed(bed)}
                              >
                                <UserCheck size={13} /> Allocate Patient
                              </button>
                            ) : isOccupied ? (
                              <button
                                type="button"
                                className="btn btn-danger-light btn-sm"
                                style={{ width: '100%', fontSize: '12.5px' }}
                                onClick={() => handleReleaseBed(bed.id)}
                              >
                                <LogOut size={13} /> Discharge Bed
                              </button>
                            ) : (
                              <button type="button" className="btn btn-secondary btn-sm" disabled style={{ width: '100%' }}>
                                In Service
                              </button>
                            )}
                          </div>
                        </div>
                      );
                    })}
                  </div>
                </div>
              );
            })
        )}
      </div>

      {/* MODAL: Allocate Bed */}
      <Modal
        isOpen={!!allocatingBed}
        onClose={() => setAllocatingBed(null)}
        title={allocatingBed ? `Allocate Bed ${allocatingBed.id} (${allocatingBed.wardName || allocatingBed.wardId})` : 'Allocate Bed'}
      >
        {allocatingBed && (
          <form onSubmit={handleAllocateBed}>
            <div style={{
              background: '#FAF8F5',
              padding: '12px 16px',
              borderRadius: '10px',
              border: '1px solid #E5E7E4',
              marginBottom: '16px',
              fontSize: '13px',
              color: '#27343A'
            }}>
              <div>Ward: <strong>{allocatingBed.wardName || allocatingBed.wardId}</strong></div>
              <div>Bed Identifier: <strong>{allocatingBed.id}</strong></div>
            </div>

            <div className="form-group">
              <label className="form-label">Select Patient to Admit *</label>
              <select
                required
                className="form-select"
                value={selectedPatientId}
                onChange={(e) => setSelectedPatientId(e.target.value)}
              >
                <option value="">-- Choose Patient from Registry --</option>
                {patients.map((p) => (
                  <option key={p.id} value={p.id}>{p.name} ({p.id}) · {p.bloodGroup}</option>
                ))}
              </select>
            </div>

            <div style={{ display: 'flex', justifyContent: 'flex-end', gap: '10px', marginTop: '20px' }}>
              <button
                type="button"
                className="btn btn-secondary"
                onClick={() => setAllocatingBed(null)}
              >
                Cancel
              </button>
              <button type="submit" className="btn btn-primary">
                Confirm Bed Allocation
              </button>
            </div>
          </form>
        )}
      </Modal>
    </div>
  );
}
