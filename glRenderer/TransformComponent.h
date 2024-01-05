#pragma once

struct Transform
{
public:
	Transform() = default;
	Transform(Quaternion& quaternion) : rotation(quaternion) {}
	Transform(const Quaternion& quaternion) : rotation(quaternion) {}

public:
	void setPosition(const glm::vec3& inPos ) { pos = inPos; }
	void addPosition(const glm::vec3& inDeltaPos) { pos += inDeltaPos; }
	void addYawRotation(float inDegree)
	{
		Rotator r = rotation.toRotator();
		r.Yaw += inDegree;
		r.Clamp();
		rotation = Quaternion(r);
	}
	void addRollRotation(float InDegree)
	{
		Rotator r = rotation.toRotator();
		r.Roll += InDegree;
		r.Clamp();
		rotation = Quaternion(r);
	}
	void addPitchRotation(float InDegree)
	{
		Rotator r = rotation.toRotator();
		r.Pitch += InDegree;
		r.Clamp();
		rotation = Quaternion(r);
	}
	constexpr void setRotation(const Rotator& inRotator) { rotation = Quaternion(inRotator); }
	// void SetRotation(const glm::mat3& InMatrix) { rotation = Quaternion(InMatrix); }
	constexpr void setRotation(const Quaternion& inQuaternion) { rotation = inQuaternion; }
	void setScale(const glm::vec3& inScale) { scale = inScale; }

	constexpr glm::vec3 getPosition() const { return pos; }
	constexpr Quaternion getRotation() const { return rotation; }
	constexpr glm::vec3 getScale() const { return scale; }

	glm::mat4 getModelingMatrix() const;
	constexpr glm::vec3 getLocalX() const { return rotation * glm::vec3(1.f, 0.f, 0.f); }
	constexpr glm::vec3 getLocalY() const { return rotation * glm::vec3(0.f, 1.f, 0.f); }
	constexpr glm::vec3 getLocalZ() const { return rotation * glm::vec3(0.f, 0.f, 1.f); }

private:
	glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f);
	Quaternion rotation;
	glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f);
};

glm::mat4 Transform::getModelingMatrix() const
{
	return glm::mat4(
		glm::vec4(getLocalX() * scale.x, 0.f),
		glm::vec4(getLocalY() * scale.y, 0.f),
		glm::vec4(getLocalZ() * scale.z, 0.f),
		glm::vec4(0.f, 0.f, 0.f, 1.f)
	);
}

