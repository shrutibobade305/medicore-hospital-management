import React, { useState } from 'react';
import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import Sidebar from './components/Sidebar';
import Navbar from './components/Navbar';
import { ToastProvider } from './components/Toast';

import Dashboard from './pages/Dashboard';
import Patients from './pages/Patients';
import Doctors from './pages/Doctors';
import Appointments from './pages/Appointments';
import EmergencyTriage from './pages/EmergencyTriage';
import BedManagement from './pages/BedManagement';
import HospitalNavigation from './pages/HospitalNavigation';
import ReportsActivity from './pages/ReportsActivity';
import SettingsAbout from './pages/SettingsAbout';

export default function App() {
  const [collapsed, setCollapsed] = useState(false);

  return (
    <ToastProvider>
      <BrowserRouter>
        <div className="app-layout">
          {/* Collapsible Left Sidebar */}
          <Sidebar isCollapsed={collapsed} setIsCollapsed={setCollapsed} />

          {/* Main Layout Area */}
          <div className="main-content">
            {/* Top Navigation Bar */}
            <Navbar collapsed={collapsed} setCollapsed={setCollapsed} />

            {/* Scrollable Main Content */}
            <div className="page-container">
              <Routes>
                <Route path="/" element={<Dashboard />} />
                <Route path="/patients" element={<Patients />} />
                <Route path="/doctors" element={<Doctors />} />
                <Route path="/appointments" element={<Appointments />} />
                <Route path="/emergency" element={<EmergencyTriage />} />
                <Route path="/beds" element={<BedManagement />} />
                <Route path="/navigation" element={<HospitalNavigation />} />
                <Route path="/reports" element={<ReportsActivity />} />
                <Route path="/settings" element={<SettingsAbout />} />
                <Route path="/visualizer" element={<Navigate to="/" replace />} />
                <Route path="/dsa-visualizer" element={<Navigate to="/" replace />} />
                <Route path="*" element={<Navigate to="/" replace />} />
              </Routes>
            </div>
          </div>
        </div>
      </BrowserRouter>
    </ToastProvider>
  );
}
