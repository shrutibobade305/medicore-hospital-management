const API_BASE = 'http://localhost:8080/api';

async function fetchJson(endpoint, options = {}) {
  const url = `${API_BASE}${endpoint}`;
  try {
    const res = await fetch(url, {
      headers: {
        'Content-Type': 'application/json',
        ...options.headers,
      },
      ...options,
    });

    const data = await res.json();
    if (!res.ok) {
      throw new Error(data.error || `HTTP ${res.status}: ${res.statusText}`);
    }
    return data;
  } catch (err) {
    console.error(`API Error on [${options.method || 'GET'}] ${endpoint}:`, err);
    throw err;
  }
}

export const api = {
  // Health
  getHealth: () => fetchJson('/health'),

  // Dashboard
  getDashboard: () => fetchJson('/dashboard'),

  // Patients (Custom Hash Table)
  getPatients: (params = {}) => {
    const q = new URLSearchParams(params).toString();
    return fetchJson(`/patients${q ? `?${q}` : ''}`);
  },
  getPatientById: (id) => fetchJson(`/patients/${encodeURIComponent(id)}`),
  createPatient: (patient) => fetchJson('/patients', { method: 'POST', body: JSON.stringify(patient) }),
  updatePatient: (id, patient) => fetchJson(`/patients/${encodeURIComponent(id)}`, { method: 'PUT', body: JSON.stringify(patient) }),
  deletePatient: (id) => fetchJson(`/patients/${encodeURIComponent(id)}`, { method: 'DELETE' }),
  getPatientVisits: (id) => fetchJson(`/patients/${encodeURIComponent(id)}/visits`),
  addPatientVisit: (id, visit) => fetchJson(`/patients/${encodeURIComponent(id)}/visits`, { method: 'POST', body: JSON.stringify(visit) }),

  // Doctors (Custom BST)
  getDoctors: (params = {}) => {
    const q = new URLSearchParams(params).toString();
    return fetchJson(`/doctors${q ? `?${q}` : ''}`);
  },
  getDoctorsSorted: () => fetchJson('/doctors/sorted'),
  getDoctorById: (id) => fetchJson(`/doctors/${encodeURIComponent(id)}`),
  createDoctor: (doctor) => fetchJson('/doctors', { method: 'POST', body: JSON.stringify(doctor) }),
  updateDoctor: (id, doctor) => fetchJson(`/doctors/${encodeURIComponent(id)}`, { method: 'PUT', body: JSON.stringify(doctor) }),
  deleteDoctor: (id) => fetchJson(`/doctors/${encodeURIComponent(id)}`, { method: 'DELETE' }),

  // Appointments (Custom FIFO Queue)
  getAppointments: (params = {}) => {
    const q = new URLSearchParams(params).toString();
    return fetchJson(`/appointments${q ? `?${q}` : ''}`);
  },
  createAppointment: (appt) => fetchJson('/appointments', { method: 'POST', body: JSON.stringify(appt) }),
  updateAppointment: (id, appt) => fetchJson(`/appointments/${encodeURIComponent(id)}`, { method: 'PUT', body: JSON.stringify(appt) }),
  cancelAppointment: (id) => fetchJson(`/appointments/${encodeURIComponent(id)}`, { method: 'DELETE' }),
  getWaitingQueue: () => fetchJson('/appointments/waiting'),

  // Emergency & Triage (Custom Binary Heap)
  getEmergencies: (all = false) => fetchJson(`/emergencies${all ? '?all=true' : ''}`),
  createEmergency: (emg) => fetchJson('/emergencies', { method: 'POST', body: JSON.stringify(emg) }),
  dispatchEmergency: () => fetchJson('/emergencies/dispatch', { method: 'POST' }),
  completeEmergency: (id) => fetchJson(`/emergencies/${encodeURIComponent(id)}/complete`, { method: 'POST' }),

  // Bed & Ward Management (Custom Undo Stack)
  getBeds: (params = {}) => {
    const q = new URLSearchParams(params).toString();
    return fetchJson(`/beds${q ? `?${q}` : ''}`);
  },
  getWards: () => fetchJson('/wards'),
  allocateBed: (bedId, patientId) => fetchJson(`/beds/${encodeURIComponent(bedId)}/allocate`, { method: 'POST', body: JSON.stringify({ patientId }) }),
  releaseBed: (bedId) => fetchJson(`/beds/${encodeURIComponent(bedId)}/release`, { method: 'POST' }),
  undoBedAction: () => fetchJson('/beds/undo', { method: 'POST' }),

  // Hospital Navigation Graph
  getHospitalMap: () => fetchJson('/navigation/map'),
  calculateRoute: (from, to) => fetchJson('/navigation/route', { method: 'POST', body: JSON.stringify({ from, to }) }),
  traverseGraph: (startId, type = 'BFS') => fetchJson('/navigation/traverse', { method: 'POST', body: JSON.stringify({ startId, type }) }),
  toggleCorridor: (from, to, closed) => fetchJson('/navigation/toggle-corridor', { method: 'POST', body: JSON.stringify({ from, to, closed }) }),

  // Activity & Audit Logs
  getActivity: () => fetchJson('/activity'),

  // System
  resetDatabase: () => fetchJson('/system/reset', { method: 'POST' }),

  // DSA Visualizer
  getDsaStructure: (structure) => fetchJson(`/dsa/${structure}`),
  runSortDemo: (array) => fetchJson('/dsa/sort-demo', { method: 'POST', body: JSON.stringify({ array }) }),
  runSearchDemo: (array, target) => fetchJson('/dsa/search-demo', { method: 'POST', body: JSON.stringify({ array, target }) }),
};
