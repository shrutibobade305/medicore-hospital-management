$target = "frontend\src\index.css"
$css = Get-Content $target -Raw
if ($css -notmatch "badge-success") {
    $extra = @"

/* ===== Additional Badge Variants ===== */
.badge-success { background: #d1fae5; color: #065f46; border: 1px solid #a7f3d0; }
.badge-danger  { background: #fee2e2; color: #991b1b; border: 1px solid #fecaca; }
.badge-info    { background: #e0f2fe; color: #0369a1; border: 1px solid #bae6fd; }
.badge-teal    { background: #ccfbf1; color: #0f766e; border: 1px solid #99f6e4; }
.badge-warning { background: #fef3c7; color: #92400e; border: 1px solid #fde68a; }
.badge-purple  { background: #ede9fe; color: #5b21b6; border: 1px solid #ddd6fe; }

@keyframes spin { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }
.animate-spin { animation: spin 1s linear infinite; }

.space-y-6 > * + * { margin-top: 1.5rem; }
.space-y-4 > * + * { margin-top: 1rem; }
.space-y-2 > * + * { margin-top: .5rem; }
.flex { display: flex; }
.flex-1 { flex: 1; }
.flex-col { flex-direction: column; }
.flex-shrink-0 { flex-shrink: 0; }
.items-center { align-items: center; }
.items-start { align-items: flex-start; }
.justify-between { justify-content: space-between; }
.justify-center { justify-content: center; }
.gap-2 { gap: .5rem; }
.gap-3 { gap: .75rem; }
.gap-4 { gap: 1rem; }
.gap-6 { gap: 1.5rem; }
.grid { display: grid; }
.grid-cols-1 { grid-template-columns: 1fr; }
@media (min-width: 768px) {
  .md-grid-cols-3 { grid-template-columns: repeat(3, 1fr); }
  .md-flex-row { flex-direction: row; }
  .md-items-center { align-items: center; }
}
.p-4 { padding: 1rem; }
.p-5 { padding: 1.25rem; }
.p-6 { padding: 1.5rem; }
.py-3 { padding-top: .75rem; padding-bottom: .75rem; }
.px-5 { padding-left: 1.25rem; padding-right: 1.25rem; }
.px-2 { padding-left: .5rem; padding-right: .5rem; }
.pt-0 { padding-top: 0; }
.mb-1 { margin-bottom: .25rem; }
.mb-3 { margin-bottom: .75rem; }
.mb-4 { margin-bottom: 1rem; }
.mt-1 { margin-top: .25rem; }
.h-4 { height: 1rem; }  .w-4 { width: 1rem; }
.h-5 { height: 1.25rem; } .w-5 { width: 1.25rem; }
.h-6 { height: 1.5rem; }  .w-6 { width: 1.5rem; }
.h-7 { height: 1.75rem; } .w-7 { width: 1.75rem; }
.h-8 { height: 2rem; }
.w-full { width: 100%; }
.max-w-xs { max-width: 20rem; }
.max-w-3xl { max-width: 48rem; }
.text-xs { font-size: 12px; }
.text-sm { font-size: 13px; }
.text-lg { font-size: 18px; }
.text-xl { font-size: 20px; }
.text-2xl { font-size: 24px; }
.font-bold { font-weight: 700; }
.font-semibold { font-weight: 600; }
.font-medium { font-weight: 500; }
.font-normal { font-weight: 400; }
.font-mono { font-family: 'JetBrains Mono', monospace; }
.tracking-tight { letter-spacing: -0.015em; }
.tracking-wider { letter-spacing: .05em; }
.leading-relaxed { line-height: 1.625; }
.uppercase { text-transform: uppercase; }
.block { display: block; }
.overflow-x-auto { overflow-x: auto; }
.overflow-hidden { overflow: hidden; }
.shadow-xl { box-shadow: var(--shadow-xl); }
.cursor-pointer { cursor: pointer; }
.text-white { color: #fff; }
.text-slate-900 { color: #0f172a; }
.text-slate-800 { color: #1e293b; }
.text-slate-700 { color: #334155; }
.text-slate-600 { color: #475569; }
.text-slate-500 { color: #64748b; }
.text-slate-400 { color: #94a3b8; }
.text-teal-700 { color: #0f766e; }
.text-teal-600 { color: #0d9488; }
.text-teal-800 { color: #115e59; }
.text-teal-300 { color: #5eead4; }
.text-teal-100 { color: #ccfbf1; }
.text-indigo-700 { color: #4338ca; }
.text-emerald-700 { color: #047857; }
.text-emerald-600 { color: #059669; }
.text-amber-500 { color: #f59e0b; }
.text-blue-600 { color: #2563eb; }
.bg-white { background-color: #fff; }
.bg-slate-900 { background-color: #0f172a; }
.bg-teal-100 { background-color: #ccfbf1; }
.bg-indigo-50 { background-color: #eef2ff; }
.bg-emerald-50 { background-color: #ecfdf5; }
.border-b { border-bottom: 1px solid var(--border); }
.border-b-2 { border-bottom-width: 2px; border-bottom-style: solid; }
.border-t { border-top: 1px solid var(--border); }
.border-t-4 { border-top-width: 4px; border-top-style: solid; }
.border-slate-200 { border-color: #e2e8f0; }
.border-slate-100 { border-color: #f1f5f9; }
.border-transparent { border-color: transparent; }
.border-teal-600 { border-color: #0d9488; }
.border-teal-100 { border-color: #ccfbf1; }
.border-t-teal-600 { border-top-color: #0d9488; }
.border-t-blue-600 { border-top-color: #2563eb; }
.border-t-indigo-600 { border-top-color: #4f46e5; }
.rounded-xl { border-radius: 12px; }
.rounded-lg { border-radius: 8px; }
.rounded-full { border-radius: 9999px; }
.transition-all { transition: all .2s ease; }
.transition-colors { transition: color .15s, background-color .15s, border-color .15s; }
.rotate-90 { transform: rotate(90deg); }
"@
    Add-Content -Path $target -Value $extra
    Write-Host "CSS utilities appended successfully."
} else {
    Write-Host "CSS utilities already present."
}
