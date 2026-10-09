import React, { useState, useEffect } from 'react';
import { 
  Navigation, 
  MapPin, 
  Route, 
  Zap, 
  AlertTriangle, 
  RotateCw, 
  Compass, 
  Footprints,
  Layers,
  ArrowRight,
  Building,
  CheckCircle2,
  XCircle,
  Clock,
  ShieldCheck
} from 'lucide-react';
import { api } from '../services/api';
import { useToast } from '../components/Toast';

export default function HospitalNavigation() {
  const [mapData, setMapData] = useState(null);
  const [loading, setLoading] = useState(true);

  // Pathfinding state
  const [startId, setStartId] = useState('ENTRANCE');
  const [endId, setEndId] = useState('OT');
  const [routeResult, setRouteResult] = useState(null);
  const [calculating, setCalculating] = useState(false);

  // Traversal state
  const [traversalType, setTraversalType] = useState('BFS');
  const [traversalResult, setTraversalResult] = useState(null);

  const { addToast } = useToast();

  const loadMap = async () => {
    try {
      setLoading(true);
      const res = await api.getHospitalMap();
      setMapData(res.data);
    } catch (err) {
      addToast(err.message || 'Failed to load hospital campus map', 'error');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadMap();
  }, []);

  const handleCalculateRoute = async () => {
    if (!startId || !endId) return;
    try {
      setCalculating(true);
      const res = await api.calculateRoute(startId, endId);
      setRouteResult(res.data);
      if (!res.data.reachable) {
        addToast('No open path found! Selected corridors are closed for maintenance.', 'error');
      } else {
        addToast(`Shortest route found: ${Math.round(res.data.totalCost)}m total travel distance`, 'success');
      }
    } catch (err) {
      addToast(err.message, 'error');
    } finally {
      setCalculating(false);
    }
  };

  const handleRunTraversal = async () => {
    try {
      const res = await api.traverseGraph(startId, traversalType);
      setTraversalResult(res.data);
      addToast(`Inspected reachable pathway across ${res.data.visitOrder.length} departments`, 'info');
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const handleToggleCorridor = async (from, to, currentClosed) => {
    try {
      const res = await api.toggleCorridor(from, to, !currentClosed);
      addToast(res.message, 'info');
      setMapData(res.data);
      if (routeResult) {
        handleCalculateRoute();
      }
    } catch (err) {
      addToast(err.message, 'error');
    }
  };

  const vertices = mapData?.vertices || [];
  const edges = mapData?.edges || [];

  const vertexMap = {};
  vertices.forEach((v) => {
    vertexMap[v.id] = v;
  });

  const pathSet = new Set();
  const pathEdgeSet = new Set();
  if (routeResult && routeResult.path) {
    routeResult.path.forEach((id) => pathSet.add(id));
    for (let i = 0; i < routeResult.path.length - 1; i++) {
      const u = routeResult.path[i];
      const v = routeResult.path[i + 1];
      pathEdgeSet.add(`${u}->${v}`);
      pathEdgeSet.add(`${v}->${u}`);
    }
  }

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
              Hospital Wayfinding & Navigation
            </h2>
            <span className="badge badge-teal">Campus Wayfinding</span>
          </div>
          <p style={{ fontSize: '13.5px', color: 'var(--text-secondary)', marginTop: '4px' }}>
            Interactive hospital floorplan, clinical department routing, and corridor status management.
          </p>
        </div>

        <button
          type="button"
          className="btn btn-secondary"
          onClick={loadMap}
          style={{ color: 'var(--primary-deep)' }}
        >
          <RotateCw size={15} /> Refresh Campus Map
        </button>
      </div>

      {/* Main Grid: Left Route Planner Controls + Right SVG Map */}
      <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(340px, 1fr))', gap: '24px', marginBottom: '26px' }}>
        
        {/* Left Column: Route Planner Form & Traversal Tools */}
        <div style={{ display: 'flex', flexDirection: 'column', gap: '20px' }}>
          
          {/* Route Planner Card */}
          <div className="card" style={{ padding: '22px 24px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '12px', marginBottom: '18px' }}>
              <div style={{
                width: '38px',
                height: '38px',
                borderRadius: '10px',
                background: 'var(--mint-tint)',
                color: 'var(--primary-deep)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center'
              }}>
                <Route size={20} />
              </div>
              <div>
                <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                  Hospital Route Planner
                </h3>
                <span className="badge badge-teal">Optimal Pathway</span>
              </div>
            </div>

            <div className="form-group">
              <label className="form-label">Origin Location / Department</label>
              <select
                className="form-select"
                value={startId}
                onChange={(e) => setStartId(e.target.value)}
              >
                {vertices.map((v) => (
                  <option key={v.id} value={v.id}>{v.name} ({v.id})</option>
                ))}
              </select>
            </div>

            <div className="form-group">
              <label className="form-label">Destination Ward / Facility</label>
              <select
                className="form-select"
                value={endId}
                onChange={(e) => setEndId(e.target.value)}
              >
                {vertices.map((v) => (
                  <option key={v.id} value={v.id}>{v.name} ({v.id})</option>
                ))}
              </select>
            </div>

            <button
              type="button"
              className="btn btn-primary"
              style={{ width: '100%', marginTop: '6px' }}
              onClick={handleCalculateRoute}
              disabled={calculating || !startId || !endId}
            >
              <Zap size={16} />
              {calculating ? 'Calculating Fastest Route...' : 'Find Optimal Route'}
            </button>

            {/* Route Output Summary */}
            {routeResult && (
              <div style={{
                marginTop: '18px',
                padding: '14px 16px',
                background: routeResult.reachable ? 'var(--mint-tint)' : '#FEF2F2',
                border: `1px solid ${routeResult.reachable ? 'var(--primary-light)' : '#FECACA'}`,
                borderRadius: '12px'
              }}>
                <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: '8px' }}>
                  <span style={{ fontSize: '12px', fontWeight: 700, textTransform: 'uppercase', color: routeResult.reachable ? 'var(--status-success)' : 'var(--status-danger)' }}>
                    {routeResult.reachable ? '✓ Route Discovered' : '✗ Route Blocked'}
                  </span>
                  {routeResult.reachable && (
                    <span style={{ fontSize: '13.5px', fontWeight: 800, color: 'var(--text-primary)' }}>
                      {Math.round(routeResult.totalCost)} meters
                    </span>
                  )}
                </div>

                {routeResult.reachable ? (
                  <div style={{ fontSize: '12.5px', color: 'var(--text-primary)', lineHeight: 1.6 }}>
                    <strong style={{ color: 'var(--primary-deep)' }}>Turn-by-turn Navigation:</strong>
                    <div style={{ display: 'flex', flexWrap: 'wrap', gap: '6px', marginTop: '6px' }}>
                      {routeResult.path.map((nodeId, idx) => (
                        <span
                          key={nodeId}
                          style={{
                            background: '#FFFFFF',
                            border: '1px solid var(--border-soft)',
                            padding: '3px 9px',
                            borderRadius: '6px',
                            fontWeight: 700,
                            color: 'var(--primary-deep)',
                            fontSize: '12px'
                          }}
                        >
                          {vertexMap[nodeId]?.name || nodeId}
                          {idx < routeResult.path.length - 1 ? ' →' : ''}
                        </span>
                      ))}
                    </div>
                  </div>
                ) : (
                  <p style={{ fontSize: '12.5px', color: 'var(--status-danger)' }}>
                    No path available due to corridor maintenance closures.
                  </p>
                )}
              </div>
            )}
          </div>

          {/* Reachability Inspector Card */}
          <div className="card" style={{ padding: '20px 24px' }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: '12px', marginBottom: '16px' }}>
              <div style={{
                width: '36px',
                height: '36px',
                borderRadius: '10px',
                background: 'var(--mint-tint)',
                color: 'var(--primary-deep)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center'
              }}>
                <Compass size={18} />
              </div>
              <div>
                <h4 style={{ fontSize: '15px', fontWeight: 700, color: 'var(--text-primary)' }}>
                  Department Connectivity Inspector
                </h4>
                <span className="badge badge-teal">Campus Reachability</span>
              </div>
            </div>

            <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '10px', marginBottom: '14px' }}>
              <button
                type="button"
                className={`btn btn-sm ${traversalType === 'BFS' ? 'btn-primary' : 'btn-secondary'}`}
                onClick={() => setTraversalType('BFS')}
              >
                Explore Connected Wards
              </button>
              <button
                type="button"
                className={`btn btn-sm ${traversalType === 'DFS' ? 'btn-primary' : 'btn-secondary'}`}
                onClick={() => setTraversalType('DFS')}
              >
                Explore Deep Pathway
              </button>
            </div>

            <button
              type="button"
              className="btn btn-secondary btn-sm"
              style={{ width: '100%', color: 'var(--primary-deep)', fontWeight: 700 }}
              onClick={handleRunTraversal}
            >
              Inspect from {vertexMap[startId]?.name || startId}
            </button>

            {traversalResult && (
              <div style={{ marginTop: '14px', padding: '12px 14px', background: 'var(--bg-canvas)', borderRadius: '10px', fontSize: '12px', border: '1px solid var(--border-soft)' }}>
                <span style={{ fontWeight: 700, color: 'var(--text-primary)' }}>Connected Sequence:</span>
                <p style={{ color: 'var(--primary-deep)', marginTop: '4px', fontFamily: 'monospace', fontWeight: 700 }}>
                  {traversalResult.visitOrder.join(' → ')}
                </p>
              </div>
            )}
          </div>

        </div>

        {/* Right Column: Interactive Hospital Campus Graph Map */}
        <div className="card" style={{ padding: '24px', display: 'flex', flexDirection: 'column' }}>
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', flexWrap: 'wrap', gap: '12px', marginBottom: '16px' }}>
            <div>
              <h3 style={{ fontSize: '16px', fontWeight: 700, color: 'var(--text-primary)' }}>
                Hospital Campus Floorplan Map
              </h3>
              <p style={{ fontSize: '12.5px', color: 'var(--text-secondary)' }}>
                Nodes represent clinical wards and facilities; lines denote connecting corridors and distances (meters).
              </p>
            </div>
            <div style={{ display: 'flex', alignItems: 'center', gap: '14px', fontSize: '12px', color: 'var(--text-secondary)' }}>
              <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                <span style={{ width: '14px', height: '4px', background: 'var(--primary-deep)', borderRadius: '2px' }}></span> Active Route
              </span>
              <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                <span style={{ width: '14px', height: '4px', background: 'var(--border-soft)', borderRadius: '2px' }}></span> Open Corridor
              </span>
              <span style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
                <span style={{ width: '14px', height: '4px', background: 'var(--status-danger)', borderStyle: 'dashed', borderRadius: '2px' }}></span> Closed
              </span>
            </div>
          </div>

          {/* SVG Map Canvas */}
          <div style={{
            flex: 1,
            background: '#FAF8F5',
            border: '1px solid var(--border-soft)',
            borderRadius: '14px',
            minHeight: '440px',
            position: 'relative',
            overflow: 'hidden',
            display: 'flex',
            alignItems: 'center',
            justifyContent: 'center'
          }}>
            {loading || !mapData ? (
              <div style={{ color: 'var(--text-secondary)', fontSize: '14px' }}>Loading Campus Map...</div>
            ) : (
              <svg viewBox="0 0 760 480" style={{ width: '100%', height: '100%', maxHeight: '520px' }}>
                {/* Corridors / Edges */}
                {edges.map((edge) => {
                  const u = vertexMap[edge.from];
                  const v = vertexMap[edge.to];
                  if (!u || !v) return null;

                  const isPath = pathEdgeSet.has(`${edge.from}->${edge.to}`);
                  const isClosed = edge.closed;

                  const midX = (u.x + v.x) / 2;
                  const midY = (u.y + v.y) / 2;

                  return (
                    <g key={edge.id} style={{ cursor: 'pointer' }} onClick={() => handleToggleCorridor(edge.from, edge.to, edge.closed)}>
                      <line
                        x1={u.x}
                        y1={u.y}
                        x2={v.x}
                        y2={v.y}
                        stroke={isClosed ? 'var(--status-danger)' : isPath ? 'var(--primary-deep)' : '#CBD5E1'}
                        strokeWidth={isPath ? 4.5 : isClosed ? 2 : 2.5}
                        strokeDasharray={isClosed ? '6,6' : 'none'}
                        strokeLinecap="round"
                      />
                      {/* Distance Badge Label */}
                      <rect
                        x={midX - 18}
                        y={midY - 10}
                        width={36}
                        height={20}
                        rx={6}
                        fill={isClosed ? '#FEE2E2' : isPath ? 'var(--mint-tint)' : '#FFFFFF'}
                        stroke={isClosed ? '#FECACA' : isPath ? 'var(--primary-deep)' : 'var(--border-soft)'}
                        strokeWidth={1}
                      />
                      <text
                        x={midX}
                        y={midY + 4}
                        textAnchor="middle"
                        fontSize={10}
                        fontWeight="700"
                        fill={isClosed ? 'var(--status-danger)' : isPath ? 'var(--primary-deep)' : 'var(--text-secondary)'}
                      >
                        {isClosed ? 'X' : `${edge.weight}m`}
                      </text>
                    </g>
                  );
                })}

                {/* Department Nodes */}
                {vertices.map((v) => {
                  const isStart = startId === v.id;
                  const isEnd = endId === v.id;
                  const isPath = pathSet.has(v.id);

                  let fillBg = '#FFFFFF';
                  let strokeColor = 'var(--border-soft)';
                  let textColor = 'var(--text-primary)';

                  if (isStart) {
                    fillBg = 'var(--primary-deep)';
                    strokeColor = 'var(--primary-teal)';
                    textColor = '#FFFFFF';
                  } else if (isEnd) {
                    fillBg = 'var(--primary-teal)';
                    strokeColor = 'var(--primary-deep)';
                    textColor = '#FFFFFF';
                  } else if (isPath) {
                    fillBg = 'var(--mint-tint)';
                    strokeColor = 'var(--primary-deep)';
                    textColor = 'var(--primary-deep)';
                  }

                  return (
                    <g
                      key={v.id}
                      transform={`translate(${v.x}, ${v.y})`}
                      style={{ cursor: 'pointer' }}
                      onClick={() => {
                        if (!startId || (startId && endId)) {
                          setStartId(v.id);
                          setEndId('');
                          setRouteResult(null);
                        } else {
                          setEndId(v.id);
                        }
                      }}
                    >
                      {/* Outer pulse circle for start/end */}
                      {(isStart || isEnd) && (
                        <circle r={28} fill={isStart ? 'rgba(79, 159, 159, 0.25)' : 'rgba(121, 189, 189, 0.25)'} />
                      )}

                      <circle
                        r={20}
                        fill={fillBg}
                        stroke={strokeColor}
                        strokeWidth={2.5}
                        style={{ filter: 'drop-shadow(0 2px 4px rgba(0,0,0,0.06))' }}
                      />

                      <text
                        textAnchor="middle"
                        dy={4}
                        fontSize={10}
                        fontWeight="800"
                        fill={isStart || isEnd ? '#FFFFFF' : 'var(--primary-deep)'}
                        fontFamily="monospace"
                      >
                        {v.id.substring(0, 3)}
                      </text>

                      {/* Department Name Label */}
                      <text
                        textAnchor="middle"
                        y={34}
                        fontSize={11.5}
                        fontWeight="700"
                        fill="var(--text-primary)"
                      >
                        {v.name}
                      </text>
                    </g>
                  );
                })}
              </svg>
            )}
          </div>

          <div style={{ marginTop: '16px', fontSize: '12px', color: 'var(--text-secondary)', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
            <span>Tip: Click any corridor line on the map to toggle open/closed maintenance status in real-time.</span>
            <span>Total Facilities: <strong style={{ color: 'var(--text-primary)' }}>{vertices.length}</strong> | Total Corridors: <strong style={{ color: 'var(--text-primary)' }}>{edges.length}</strong></span>
          </div>
        </div>

      </div>
    </div>
  );
}
