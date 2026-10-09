// Mapping of doctor portraits to local assets and curated medical portrait photography

export const DOCTOR_IMAGES = {
  'DOC-101': {
    src: '/doctors/doc-1.png',
    fallback: 'https://images.unsplash.com/photo-1622253692010-333f2da6031d?auto=format&fit=crop&w=400&q=80',
    title: 'Dr. Rajesh Rao',
    department: 'Cardiology'
  },
  'DOC-102': {
    src: '/doctors/doc-2.png',
    fallback: 'https://images.unsplash.com/photo-1594824813589-3221379db735?auto=format&fit=crop&w=400&q=80',
    title: 'Dr. Shalini Gupta',
    department: 'Neurology'
  },
  'DOC-103': {
    src: 'https://images.unsplash.com/photo-1537368910025-700350fe46c7?auto=format&fit=crop&w=400&q=80',
    fallback: '/doctors/doc-1.png',
    title: 'Dr. Amitava Roy',
    department: 'Orthopedics'
  },
  'DOC-104': {
    src: '/doctors/doctor-hero-portrait.jpg',
    fallback: 'https://images.unsplash.com/photo-1559839734-2b71ea197ec2?auto=format&fit=crop&w=400&q=80',
    title: 'Dr. Preethi Hegde',
    department: 'Obstetrics & Gyn'
  },
  'DOC-105': {
    src: 'https://images.unsplash.com/photo-1612349317150-e413f6a5b16d?auto=format&fit=crop&w=400&q=80',
    fallback: '/doctors/doc-1.png',
    title: 'Dr. Farhan Siddiqui',
    department: 'Emergency Medicine'
  },
  'DOC-106': {
    src: 'https://images.unsplash.com/photo-1582750433449-648ed127bb54?auto=format&fit=crop&w=400&q=80',
    fallback: '/doctors/doc-2.png',
    title: 'Dr. Kavita Menon',
    department: 'Pulmonology'
  }
};

export function getDoctorImage(id, name) {
  if (id && DOCTOR_IMAGES[id]) {
    return DOCTOR_IMAGES[id].src;
  }
  // Fallback by ID index
  const keys = Object.keys(DOCTOR_IMAGES);
  if (keys.length > 0) {
    const idx = (id ? id.split('').reduce((acc, c) => acc + c.charCodeAt(0), 0) : 0) % keys.length;
    return DOCTOR_IMAGES[keys[idx]].src;
  }
  return '/doctors/doctor-hero-portrait.jpg';
}
