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

    // Referenced Image Sequence
    if (measurementInformation.referencedImageSequence.size() == 1)
    {
        mrd::ReferencedImageSequenceType referencedImage;
        referencedImage.referenced_sop_instance_uid = measurementInformation.referencedImageSequence[0].referencedSOPInstanceUID;
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
