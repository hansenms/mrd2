#include "generated/binary/protocols.h"
#include <filesystem>
#include <iostream>
#include <exception>
#include <ismrmrd/dataset.h>
#include <ismrmrd/serialization_iostream.h>

yardl::Date date_from_string(const std::string &s)
{
    std::stringstream ss{s};
    yardl::Date d;
#if __cplusplus < 202002L
    ss >> date::parse("%F", d);
#else
    date::local_days ld;
    ss >> date::parse("%F", ld);
    d = yardl::Date(ld.time_since_epoch());
#endif
    return d;
}

yardl::Time time_from_string(const std::string &s)
{
    std::stringstream ss{s};
    yardl::Time t;
#if __cplusplus < 202002L
    ss >> date::parse("%F", t);
#else
    date::local_days ld;
    ss >> date::parse("%F", ld);
    t = yardl::Date(ld.time_since_epoch());
#endif

    if (ss.fail())
    {
        throw std::runtime_error("invalid date format");
    }
    return t;
};

mrd::SubjectInformationType convert(ISMRMRD::SubjectInformation &subjectInformation)
{
    mrd::SubjectInformationType s;

    if (subjectInformation.patientName)
    {
        s.patient_name = *subjectInformation.patientName;
    }

    if (subjectInformation.patientWeight_kg)
    {
        s.patient_weightkg = *subjectInformation.patientWeight_kg;
    }

    if (subjectInformation.patientHeight_m)
    {
        s.patient_heightm = *subjectInformation.patientHeight_m;
    }

    if (subjectInformation.patientID)
    {
        s.patient_id = *subjectInformation.patientID;
    }

    if (subjectInformation.patientBirthdate)
    {
        s.patient_birthdate = date_from_string(*subjectInformation.patientBirthdate);
    }

    if (subjectInformation.patientGender)
    {
        if (*subjectInformation.patientGender == std::string("M"))
        {
            s.patient_gender = mrd::PatientGender::kM;
        }
        else if (*subjectInformation.patientGender == std::string("F"))
        {
            s.patient_gender = mrd::PatientGender::kF;
        }
        else if (*subjectInformation.patientGender == std::string("O"))
        {
            s.patient_gender = mrd::PatientGender::kO;
        }
        else
        {
            // throw runtime exception
            throw std::runtime_error("Unknown Gender");
        }
    }

    return s;
}

mrd::StudyInformationType convert(ISMRMRD::StudyInformation &studyInformation)
{
    mrd::StudyInformationType s;

    if (studyInformation.studyDate)
    {
        s.study_date = date_from_string(*studyInformation.studyDate);
    }

    if (studyInformation.studyTime)
    {
        s.study_time = time_from_string(*studyInformation.studyTime);
    }

    if (studyInformation.studyID)
    {
        s.study_id = *studyInformation.studyID;
    }

    if (studyInformation.accessionNumber)
    {
        s.accession_number = *studyInformation.accessionNumber;
    }

    if (studyInformation.referringPhysicianName)
    {
        s.referring_physician_name = *studyInformation.referringPhysicianName;
    }

    if (studyInformation.studyDescription)
    {
        s.study_description = *studyInformation.studyDescription;
    }

    if (studyInformation.studyInstanceUID)
    {
        s.study_instance_uid = *studyInformation.studyInstanceUID;
    }

    if (studyInformation.bodyPartExamined)
    {
        s.body_part_examined = *studyInformation.bodyPartExamined;
    }

    return s;
}

// Convert string to PatientPosition
mrd::PatientPosition patient_position_from_string(std::string &s)
{
    if (s == "HFP")
    {
        return mrd::PatientPosition::kHFP;
    }
    else if (s == "HFS")
    {
        return mrd::PatientPosition::kHFS;
    }
    else if (s == "HFDR")
    {
        return mrd::PatientPosition::kHFDR;
    }
    else if (s == "HFDL")
    {
        return mrd::PatientPosition::kHFDL;
    }
    else if (s == "FFP")
    {
        return mrd::PatientPosition::kFFP;
    }
    else if (s == "FFS")
    {
        return mrd::PatientPosition::kFFS;
    }
    else if (s == "FFDR")
    {
        return mrd::PatientPosition::kFFDR;
    }
    else if (s == "FFDL")
    {
        return mrd::PatientPosition::kFFDL;
    }
    else
    {
        throw std::runtime_error("Unknown Patient Position");
    }
}

mrd::ThreeDimensionalFloat convert(ISMRMRD::threeDimensionalFloat &threeDimensionalFloat)
{
    mrd::ThreeDimensionalFloat t;
    t.x = threeDimensionalFloat.x;
    t.y = threeDimensionalFloat.y;
    t.z = threeDimensionalFloat.z;
    return t;
}

mrd::MeasurementDependencyType convert(ISMRMRD::MeasurementDependency &measurementDependency)
{
    mrd::MeasurementDependencyType m;
    m.measurement_id = measurementDependency.measurementID;
    m.dependency_type = measurementDependency.dependencyType;
    return m;
}

mrd::MeasurementInformationType convert(ISMRMRD::MeasurementInformation &measurementInformation)
{
    mrd::MeasurementInformationType m;

    if (measurementInformation.measurementID)
    {
        m.measurement_id = *measurementInformation.measurementID;
    }

    if (measurementInformation.seriesDate)
    {
        m.series_date = date_from_string(*measurementInformation.seriesDate);
    }

    if (measurementInformation.seriesTime)
    {
        m.series_time = time_from_string(*measurementInformation.seriesTime);
    }

    m.patient_position = patient_position_from_string(measurementInformation.patientPosition);

    if (measurementInformation.relativeTablePosition)
    {
        m.relative_table_position = convert(*measurementInformation.relativeTablePosition);
    }

    if (measurementInformation.initialSeriesNumber)
    {
        m.initial_series_number = *measurementInformation.initialSeriesNumber;
    }

    if (measurementInformation.protocolName)
    {
        m.protocol_name = *measurementInformation.protocolName;
    }

    if (measurementInformation.sequenceName)
    {
        m.sequence_name = *measurementInformation.sequenceName;
    }

    if (measurementInformation.seriesDescription)
    {
        m.series_description = *measurementInformation.seriesDescription;
    }

    for (auto &dependency : measurementInformation.measurementDependency)
    {
        m.measurement_dependency.push_back(convert(dependency));
    }

    if (measurementInformation.seriesInstanceUIDRoot)
    {
        m.series_instance_uid_root = *measurementInformation.seriesInstanceUIDRoot;
    }

    if (measurementInformation.frameOfReferenceUID)
    {
        m.frame_of_reference_uid = *measurementInformation.frameOfReferenceUID;
    }

    // TODO: Check this, schema is unclear
    if (measurementInformation.referencedImageSequence.size() > 0)
    {
        mrd::ReferencedImageSequenceType referencedImage;
        for (auto &image : measurementInformation.referencedImageSequence)
        {
            referencedImage.referenced_sop_instance_uid.push_back(image.referencedSOPInstanceUID);
        }
        m.referenced_image_sequence = referencedImage;
    }

    if (measurementInformation.referencedImageSequence.size() > 1)
    {
        throw std::runtime_error("Referenced Image Sequence with more than one item is not supported");
    }

    return m;
}

mrd::AcquisitionSystemInformationType convert(ISMRMRD::AcquisitionSystemInformation &a)
{
    mrd::AcquisitionSystemInformationType asi;

    if (a.systemVendor)
    {
        asi.system_vendor = *a.systemVendor;
    }

    if (a.systemModel)
    {
        asi.system_model = *a.systemModel;
    }

    if (a.systemFieldStrength_T)
    {
        asi.system_field_strength_t = *a.systemFieldStrength_T;
    }

    if (a.relativeReceiverNoiseBandwidth)
    {
        asi.relative_receiver_noise_bandwidth = *a.relativeReceiverNoiseBandwidth;
    }

    if (a.receiverChannels)
    {
        asi.receiver_channels = *a.receiverChannels;
    }

    if (a.coilLabel.size() > 0)
    {
        for (auto &c : a.coilLabel)
        {
            mrd::CoilLabelType cl;
            cl.coil_name = c.coilName;
            cl.coil_number = c.coilNumber;
            asi.coil_label.push_back(cl);
        }
    }

    if (a.institutionName)
    {
        asi.institution_name = *a.institutionName;
    }

    if (a.stationName)
    {
        asi.station_name = *a.stationName;
    }

    if (a.deviceID)
    {
        asi.device_id = *a.deviceID;
    }

    if (a.deviceSerialNumber)
    {
        asi.device_serial_number = *a.deviceSerialNumber;
    }

    return asi;
}

mrd::ExperimentalConditionsType convert(ISMRMRD::ExperimentalConditions &e)
{
    mrd::ExperimentalConditionsType ec;
    ec.h1resonance_frequency_hz = e.H1resonanceFrequency_Hz;
    return ec;
}

// convert ISMRMRD::MatrixSize to mrd::MatrixSizeType
// Schema:
// MatrixSizeType: !record
//   fields:
//     x: uint
//     y: uint
//     z: uint
mrd::MatrixSizeType convert(ISMRMRD::MatrixSize &m)
{
    mrd::MatrixSizeType matrixSize;
    matrixSize.x = m.x;
    matrixSize.y = m.y;
    matrixSize.z = m.z;
    return matrixSize;
}

// convert ISMRMRD::FieldOfView to mrd::FieldOfViewType
// Schema:
// FieldOfViewmm: !record
//   fields:
//     x: float
//     y: float
//     z: float
mrd::FieldOfViewmm convert(ISMRMRD::FieldOfView_mm &f)
{
    mrd::FieldOfViewmm fieldOfView;
    fieldOfView.x = f.x;
    fieldOfView.y = f.y;
    fieldOfView.z = f.z;
    return fieldOfView;
}

// Convert ISMRMRD::EncodingSpace to mrd::EncodingSpaceType
// Schema:
// EncodingSpaceType: !record
//   fields:
//     matrixSize: MatrixSizeType
//     fieldOfViewmm: FieldOfViewmm
mrd::EncodingSpaceType convert(ISMRMRD::EncodingSpace &e)
{
    mrd::EncodingSpaceType encodingSpace;
    encodingSpace.matrix_size = convert(e.matrixSize);
    encodingSpace.field_of_viewmm = convert(e.fieldOfView_mm);
    return encodingSpace;
}

// Converts ISMRMRD::Limit to mrd::LimitType
// Schema:
// LimitType: !record
//   fields:
//     minimum: uint
//     maximum: uint
//     center: uint
mrd::LimitType convert(ISMRMRD::Limit &l)
{
    mrd::LimitType limit;
    limit.minimum = l.minimum;
    limit.maximum = l.maximum;
    limit.center = l.center;
    return limit;
}

// Comvert ISMRMRD::EncodingLimits to mrd::EncodingLimitsType
// Schema:
// EncodingLimitsType: !record
//   fields:
//     kspaceencodingstep0: LimitType?
//     kspaceencodingstep1: LimitType?
//     kspaceencodingstep2: LimitType?
//     average: LimitType?
//     slice: LimitType?
//     contrast: LimitType?
//     phase: LimitType?
//     repetition: LimitType?
//     set: LimitType?
//     segment: LimitType?
//     user0: LimitType?
//     user1: LimitType?
//     user2: LimitType?
//     user3: LimitType?
//     user4: LimitType?
//     user5: LimitType?
//     user6: LimitType?
//     user7: LimitType?
mrd::EncodingLimitsType convert(ISMRMRD::EncodingLimits &e)
{
    mrd::EncodingLimitsType encodingLimits;

    if (e.kspace_encoding_step_0)
    {
        encodingLimits.kspaceencodingstep0 = convert(*e.kspace_encoding_step_0);
    }

    if (e.kspace_encoding_step_1)
    {
        encodingLimits.kspaceencodingstep1 = convert(*e.kspace_encoding_step_1);
    }

    if (e.kspace_encoding_step_2)
    {
        encodingLimits.kspaceencodingstep2 = convert(*e.kspace_encoding_step_2);
    }

    if (e.average)
    {
        encodingLimits.average = convert(*e.average);
    }

    if (e.slice)
    {
        encodingLimits.slice = convert(*e.slice);
    }

    if (e.contrast)
    {
        encodingLimits.contrast = convert(*e.contrast);
    }

    if (e.phase)
    {
        encodingLimits.phase = convert(*e.phase);
    }

    if (e.repetition)
    {
        encodingLimits.repetition = convert(*e.repetition);
    }

    if (e.set)
    {
        encodingLimits.set = convert(*e.set);
    }

    if (e.segment)
    {
        encodingLimits.segment = convert(*e.segment);
    }

    if (e.user[0])
    {
        encodingLimits.user0 = convert(*e.user[0]);
    }

    if (e.user[1])
    {
        encodingLimits.user1 = convert(*e.user[1]);
    }

    if (e.user[2])
    {
        encodingLimits.user2 = convert(*e.user[2]);
    }

    if (e.user[3])
    {
        encodingLimits.user3 = convert(*e.user[3]);
    }

    if (e.user[4])
    {
        encodingLimits.user4 = convert(*e.user[4]);
    }

    if (e.user[5])
    {
        encodingLimits.user5 = convert(*e.user[5]);
    }

    if (e.user[6])
    {
        encodingLimits.user6 = convert(*e.user[6]);
    }

    if (e.user[7])
    {
        encodingLimits.user7 = convert(*e.user[7]);
    }

    return encodingLimits;
}

// Convert ISMRMRD::UserParameterLong to mrd::UserParameterLongType
// UserParameterLongType: !record
//   fields:
//     name: string
//     value: long
mrd::UserParameterLongType convert(ISMRMRD::UserParameterLong &u)
{
    mrd::UserParameterLongType userParameterLong;
    userParameterLong.name = u.name;
    userParameterLong.value = u.value;
    return userParameterLong;
}

// Convert ISMRMRD::UserParameterDouble to mrd::UserParameterDoubleType
// UserParameterDoubleType: !record
//   fields:
//     name: string
//     value: double
mrd::UserParameterDoubleType convert(ISMRMRD::UserParameterDouble &u)
{
    mrd::UserParameterDoubleType userParameterDouble;
    userParameterDouble.name = u.name;
    userParameterDouble.value = u.value;
    return userParameterDouble;
}

// Convert ISMRMRD::UserParameterString to mrd::UserParameterStringType
// UserParameterStringType: !record
//   fields:
//     name: string
//     value: string
mrd::UserParameterStringType convert(ISMRMRD::UserParameterString &u)
{
    mrd::UserParameterStringType userParameterString;
    userParameterString.name = u.name;
    userParameterString.value = u.value;
    return userParameterString;
}

// Convert ISMRMRD::TrajectoryDescription to mrd::TrajectoryDescriptionType
// TrajectoryDescriptionType: !record
//   fields:
//     identifier: string
//     userParameterLong: !vector
//       items: UserParameterLongType
//     userParameterDouble: !vector
//       items: UserParameterDoubleType
//     userParameterString: !vector
//       items: UserParameterStringType
//     comment: string?
mrd::TrajectoryDescriptionType convert(ISMRMRD::TrajectoryDescription &t)
{
    mrd::TrajectoryDescriptionType trajectoryDescription;
    trajectoryDescription.identifier = t.identifier;

    for (auto &u : t.userParameterLong)
    {
        trajectoryDescription.user_parameter_long.push_back(convert(u));
    }

    for (auto &u : t.userParameterDouble)
    {
        trajectoryDescription.user_parameter_double.push_back(convert(u));
    }

    for (auto &u : t.userParameterString)
    {
        trajectoryDescription.user_parameter_string.push_back(convert(u));
    }

    if (t.comment)
    {
        trajectoryDescription.comment = *t.comment;
    }

    return trajectoryDescription;
}

// Convert ISMRMRD::AccellerationFactor to mrd::AccelerationFactorType
// AccelerationFactorType: !record
//   fields:
//     kspaceencodingstep1: uint
//     kspaceencodingstep2: uint
mrd::AccelerationFactorType convert(ISMRMRD::AccelerationFactor &a)
{
    mrd::AccelerationFactorType accelerationFactor;
    accelerationFactor.kspaceencodingstep1 = a.kspace_encoding_step_1;
    accelerationFactor.kspaceencodingstep2 = a.kspace_encoding_step_2;
    return accelerationFactor;
}

// Convert ISMRMRD::CalibrationMode to mrd::CalibrationModeType
// CalibrationMode: !enum
//     values:
//     - embedded
//     - interleaved
//     - separate
//     - external
//     - other
mrd::CalibrationMode calibration_mode_from_string(std::string &m)
{
    if (m == "embedded")
    {
        return mrd::CalibrationMode::kEmbedded;
    }
    else if (m == "interleaved")
    {
        return mrd::CalibrationMode::kInterleaved;
    }
    else if (m == "separate")
    {
        return mrd::CalibrationMode::kSeparate;
    }
    else if (m == "external")
    {
        return mrd::CalibrationMode::kExternal;
    }
    else if (m == "other")
    {
        return mrd::CalibrationMode::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown CalibrationMode: " + m);
    }
}

// Convert ISMRMRD::InterleavingDimension to mrd::InterleavingDimensionType
// InterleavingDimension: !enum
//     values:
//     - phase
//     - repetition
//     - contrast
//     - average
//     - other
mrd::InterleavingDimension interleaving_dimension_from_string(std::string &s)
{
    if (s == "phase")
    {
        return mrd::InterleavingDimension::kPhase;
    }
    else if (s == "repetition")
    {
        return mrd::InterleavingDimension::kRepetition;
    }
    else if (s == "contrast")
    {
        return mrd::InterleavingDimension::kContrast;
    }
    else if (s == "average")
    {
        return mrd::InterleavingDimension::kAverage;
    }
    else if (s == "other")
    {
        return mrd::InterleavingDimension::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown InterleavingDimension: " + s);
    }
}

// Convert ISMRMRD::MultibandSpacing to mrd::MultibandSpacingType
// MultibandSpacingType: !record
//   fields:
//     dZ: float
mrd::MultibandSpacingType convert(ISMRMRD::MultibandSpacing &m)
{
    mrd::MultibandSpacingType multibandSpacing;
    for (auto s : m.dZ)
    {
        multibandSpacing.d_z.push_back(s);
    }
    return multibandSpacing;
}

// Calibration from string
// Calibration: !enum
//     values:
//     - separable2D
//     - full3D
//     - other
mrd::Calibration convert(ISMRMRD::MultibandCalibrationType &m)
{
    if (m == ISMRMRD::MultibandCalibrationType::SEPARABLE2D)
    {
        return mrd::Calibration::kSeparable2D;
    }
    else if (m == ISMRMRD::MultibandCalibrationType::FULL3D)
    {
        return mrd::Calibration::kFull3D;
    }
    else if (m == ISMRMRD::MultibandCalibrationType::OTHER)
    {
        return mrd::Calibration::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown Calibration");
    }
}

// Convert ISMRMRD::MultiBand to mrd::MultibandType
// MultibandType: !record
//   fields:
//     spacing: !vector
//       items: MultibandSpacingType
//     deltaKz: float
//     multibandfactor: uint
//     calibration: Calibration
//     calibrationencoding: uint64
mrd::MultibandType convert(ISMRMRD::Multiband &m)
{
    mrd::MultibandType multiband;
    for (auto s : m.spacing)
    {
        multiband.spacing.push_back(convert(s));
    }
    multiband.delta_kz = m.deltaKz;
    multiband.multibandfactor = m.multiband_factor;
    multiband.calibration = convert(m.calibration);
    multiband.calibrationencoding = m.calibration_encoding;
    return multiband;
}

// Convert ISMRMRD::ParallelImaging to mrd::ParallelImagingType
// ParallelImagingType: !record
//   fields:
//     accelerationFactor: AccelerationFactorType
//     calibrationMode: CalibrationMode?
//     interleavingDimension: InterleavingDimension?
//     multiband: MultibandType?
mrd::ParallelImagingType convert(ISMRMRD::ParallelImaging &p)
{
    mrd::ParallelImagingType parallelImaging;
    parallelImaging.acceleration_factor = convert(p.accelerationFactor);
    if (p.calibrationMode)
    {
        parallelImaging.calibration_mode = calibration_mode_from_string(*p.calibrationMode);
    }
    if (p.interleavingDimension)
    {
        parallelImaging.interleaving_dimension = interleaving_dimension_from_string(*p.interleavingDimension);
    }
    if (p.multiband)
    {
        parallelImaging.multiband = convert(*p.multiband);
    }
    return parallelImaging;
}

// Convert ISMRMRD::Encoding to mrd::EncodingType
// Schema:
// EncodingType: !record
//   fields:
//     encodedSpace: EncodingSpaceType
//     reconSpace: EncodingSpaceType
//     encodingLimits: EncodingLimitsType
//     trajectory: Trajectory
//     trajectoryDescription: TrajectoryDescriptionType?
//     parallelImaging: ParallelImagingType?
//     echoTrainLength: long?
mrd::EncodingType convert(ISMRMRD::Encoding &e)
{
    mrd::EncodingType encoding;

    encoding.encoded_space = convert(e.encodedSpace);
    encoding.recon_space = convert(e.reconSpace);
    encoding.encoding_limits = convert(e.encodingLimits);

    if (e.trajectory == ISMRMRD::TrajectoryType::CARTESIAN)
    {
        encoding.trajectory = mrd::Trajectory::kCartesian;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::EPI)
    {
        encoding.trajectory = mrd::Trajectory::kEpi;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::RADIAL)
    {
        encoding.trajectory = mrd::Trajectory::kRadial;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::GOLDENANGLE)
    {
        encoding.trajectory = mrd::Trajectory::kGoldenangle;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::SPIRAL)
    {
        encoding.trajectory = mrd::Trajectory::kSpiral;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::OTHER)
    {
        encoding.trajectory = mrd::Trajectory::kOther;
    }
    else
    {
        // throw error
        throw std::runtime_error("Unknown trajectory type");
    }

    if (e.trajectoryDescription)
    {
        encoding.trajectory_description = convert(*e.trajectoryDescription);
    }

    if (e.parallelImaging)
    {
        encoding.parallel_imaging = convert(*e.parallelImaging);
    }

    if (e.echoTrainLength)
    {
        encoding.echo_train_length = *e.echoTrainLength;
    }

    return encoding;
}

mrd::Header convert(ISMRMRD::IsmrmrdHeader &hdr)
{
    mrd::Header h;

    if (hdr.version)
    {
        h.version = *hdr.version;
    }

    if (hdr.subjectInformation)
    {
        h.subject_information = convert(*hdr.subjectInformation);
    }

    if (hdr.studyInformation)
    {
        h.study_information = convert(*hdr.studyInformation);
    }

    if (hdr.measurementInformation)
    {
        h.measurement_information = convert(*hdr.measurementInformation);
    }

    if (hdr.acquisitionSystemInformation)
    {
        h.acquisition_system_information = convert(*hdr.acquisitionSystemInformation);
    }

    h.experimental_conditions = convert(hdr.experimentalConditions);

    if (hdr.encoding.size() > 0)
    {
        for (auto e : hdr.encoding)
        {
            h.encoding.push_back(convert(e));
        }
    }
    else
    {
        throw std::runtime_error("No encoding found in ISMRMRD header");
    }

    return h;
}

int main()
{
    ISMRMRD::IStreamView rs(std::cin);
    ISMRMRD::ProtocolDeserializer deserializer(rs);
    mrd::binary::MrdWriter w(std::cout);

    // Some reconstructions return the header but it is not required.
    if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_HEADER)
    {
        ISMRMRD::IsmrmrdHeader hdr;
        deserializer.deserialize(hdr);
        w.WriteHeader(convert(hdr));
    }
    else
    {
        std::cerr << "No ISMRMRD header found in input stream." << std::endl;
        return 1;
    }

    w.EndData();

    return 0;
}
